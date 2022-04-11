#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"

class NetworkController : public QObject
{
	Q_OBJECT

public:
	explicit NetworkController(QSslSocket* socket, QObject* parent = nullptr);

	void start(); // slot

private:
	void readRequest();
	void writeResponse();

	QSharedPointer<Response> handleRequest(QSharedPointer<Request> request);
	RegisterResponse* handleRequest(const RegisterRequest* request);
	LoginResponse* handleRequest(const LoginRequest* request);
	LogoutResponse* handleRequest(const LogoutRequest* request);
	DeleteUserResponse* handleRequest(const DeleteUserRequest* request);

private:
	QTimer timer;
	QScopedPointer<QSslSocket> socket;
	QByteArray receiveBuffer;
};
