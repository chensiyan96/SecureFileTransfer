#pragma once

#include "../SecureFileTransfer/NetworkControllerBase.h"

class NetworkController : public NetworkControllerBase
{
	Q_OBJECT

public:
	explicit NetworkController(QObject* parent = nullptr);

	void start(QSslSocket* socket); // slot

private:
	QSharedPointer<Response> handleRequest(QSharedPointer<Request> request);
	RegisterResponse* handleRequest(const RegisterRequest* request);
	LoginResponse* handleRequest(const LoginRequest* request);

private:
	QByteArray receiveBuffer;
};
