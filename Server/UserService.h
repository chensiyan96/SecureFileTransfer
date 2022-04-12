#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"
#include"../DatabaseAccess/User.h"

class UserService : public QObject
{
	Q_OBJECT

public:
	explicit UserService(QObject* parent = nullptr);

	RegisterResponse* handleRequest(const RegisterRequest* request);
	LoginResponse* handleRequest(const LoginRequest* request);
	LogoutResponse* handleRequest(const LogoutRequest* request);
	DeleteUserResponse* handleRequest(const DeleteUserRequest* request);

signals:
	void start();

	void insertUserSignal(dao::User* user);
	void selectUserByIdSignal(int id, dao::User* user);
	void selectUserByNameSignal(QString username, dao::User* user);
	void deleteUserByIdSignal(int id);

private:
	void startDatabaseConnector();

	void insertUserSlot(dao::User* user);
	void selectUserByIdSlot(int id, dao::User* user);
	void selectUserByNameSlot(QString username, dao::User* user);
	void deleteUserByIdSlot(int id);

private:
	QThread databaseThread;
	QSemaphore semaphore;
	QMutex mutex;
	volatile bool lastResult = false;
};
