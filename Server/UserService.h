#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"
#include"../DatabaseAccess/User.h"

class UserService : public QObject
{
	Q_OBJECT

public:
	explicit UserService(QObject* parent = nullptr);

	RegisterResponse* handleRequest(QSslSocket* socket, const RegisterRequest* request);
	LoginResponse* handleRequest(QSslSocket* socket, const LoginRequest* request);
	LogoutResponse* handleRequest(QSslSocket* socket, const LogoutRequest* request);
	DeleteUserResponse* handleRequest(QSslSocket* socket, const DeleteUserRequest* request);

	bool isLogin(QSslSocket* socket);

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
	QMap<QSslSocket*, dao::User> loginMap;
	QThread databaseThread;
	QSemaphore semaphore;
	QMutex mutex;
	volatile bool lastResult = false;
};
