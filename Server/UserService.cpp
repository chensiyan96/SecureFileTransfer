#include "stdafx.h"

#include "UserService.h"

#include"../DatabaseAccess/DatabaseAccess.h"

UserService::UserService(QObject *parent)
	: QObject(parent)
{
	moveToThread(&databaseThread);
	databaseThread.start();

	connect(this, &UserService::insertUserSignal, this, &UserService::insertUserSlot);
	connect(this, &UserService::selectUserByIdSignal, this, &UserService::selectUserByIdSlot);
	connect(this, &UserService::selectUserByNameSignal, this, &UserService::selectUserByNameSlot);
	connect(this, &UserService::deleteUserByIdSignal, this, &UserService::deleteUserByIdSlot);

	connect(this, &UserService::start, this, &UserService::startDatabaseConnector);
	emit start();
}

RegisterResponse* UserService::handleRequest(const RegisterRequest* request)
{
	auto response = new RegisterResponse(request->id);
	dao::User user;
	user.username = request->username;
	user.salt.resize(16);
	for (int i = 0; i < user.salt.size(); i++)
	{
		user.salt[i] = char(qrand());
	}

	// 密码加盐保存
	user.password = request->password;
	user.password.append(user.salt);
	user.password = QCryptographicHash::hash(user.password, QCryptographicHash::Algorithm::Sha256);

	// 访问数据库
	mutex.lock();
	emit insertUserSignal(&user);
	semaphore.acquire(1);
	if (!lastResult)
	{
		mutex.unlock();
		response->result = RegisterResponse::Result::REPEAT_USERNAME;
		return response;
	}
	mutex.unlock();
	return response;
}

LoginResponse* UserService::handleRequest(const LoginRequest* request)
{
	auto response = new LoginResponse(request->id);
	if (request->username.size() > 255 || request->username.size() == 0)
	{
		response->result = LoginResponse::Result::INVALID_ARGUMENT;
		return response;
	}
	dao::User user;

	// 访问数据库
	mutex.lock();
	emit selectUserByNameSignal(request->username, &user);
	semaphore.acquire(1);
	if (!lastResult)
	{
		mutex.unlock();
		response->result = LoginResponse::Result::WRONG_USERNAME_OR_PASSWORD;
		return response;
	}
	mutex.unlock();

	// 检验密码
	auto password = request->password;
	password.append(user.salt);
	password = QCryptographicHash::hash(password, QCryptographicHash::Algorithm::Sha256);
	if (password != user.password)
	{
		response->result = LoginResponse::Result::WRONG_USERNAME_OR_PASSWORD;
		return response;
	}
	return response;
}

LogoutResponse* UserService::handleRequest(const LogoutRequest* request)
{
	auto response = new LogoutResponse(request->id);
	return response;
}

DeleteUserResponse* UserService::handleRequest(const DeleteUserRequest* request)
{
	auto response = new DeleteUserResponse(request->id);
	return response;
}

void UserService::startDatabaseConnector()
{
	connectToDatabase();
}

void UserService::insertUserSlot(dao::User* user)
{
	lastResult = dao::insertUser(*user);
	semaphore.release(1);
}

void UserService::selectUserByIdSlot(int id, dao::User* user)
{
	lastResult = dao::selectUserById(id, *user);
	semaphore.release(1);
}

void UserService::selectUserByNameSlot(QString username, dao::User* user)
{
	lastResult = dao::selectUserByName(username, *user);
	semaphore.release(1);
}

void UserService::deleteUserByIdSlot(int id)
{
	lastResult = dao::deleteUserById(id);
	semaphore.release(1);
}
