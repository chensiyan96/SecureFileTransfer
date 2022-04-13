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

RegisterResponse* UserService::handleRequest(QSslSocket* socket, const RegisterRequest* request)
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

LoginResponse* UserService::handleRequest(QSslSocket* socket, const LoginRequest* request)
{
	auto response = new LoginResponse(request->id);
	mutex.lock();
	if (loginMap.contains(socket))
	{
		auto response = new LoginResponse(request->id);
		response->result = LoginResponse::Result::LOGOUT_FIRST;
		return response;
	}

	// 访问数据库
	dao::User user;
	emit selectUserByNameSignal(request->username, &user);
	semaphore.acquire(1);
	if (!lastResult)
	{
		mutex.unlock();
		response->result = LoginResponse::Result::WRONG_USERNAME_OR_PASSWORD;
		return response;
	}

	// 检验密码
	auto password = request->password;
	password.append(user.salt);
	password = QCryptographicHash::hash(password, QCryptographicHash::Algorithm::Sha256);
	if (password != user.password)
	{
		mutex.unlock();
		response->result = LoginResponse::Result::WRONG_USERNAME_OR_PASSWORD;
		return response;
	}

	// 登录成功
	loginMap.insert(socket, user);
	mutex.unlock();
	return response;
}

LogoutResponse* UserService::handleRequest(QSslSocket* socket, const LogoutRequest* request)
{
	auto response = new LogoutResponse(request->id);
	mutex.lock();
	if (loginMap.remove(socket) == 0)
	{
		response->result = LogoutResponse::Result::DID_NOT_LOGIN;
	}
	mutex.unlock();
	return response;
}

DeleteUserResponse* UserService::handleRequest(QSslSocket* socket, const DeleteUserRequest* request)
{
	auto response = new DeleteUserResponse(request->id);
	mutex.lock();
	auto iter = loginMap.find(socket);
	if (iter == loginMap.end())
	{
		response->result = DeleteUserResponse::Result::DID_NOT_LOGIN;
	}
	else
	{
		// 访问数据库
		emit deleteUserByIdSignal(iter.value().id);
		semaphore.acquire(1);
	}
	mutex.unlock();
	return response;
}

bool UserService::isLogin(QSslSocket* socket)
{
	mutex.lock();
	bool ret = loginMap.contains(socket);
	mutex.unlock();
	return ret;
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
