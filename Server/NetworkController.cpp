#include "stdafx.h"

#include "NetworkController.h"

NetworkController::NetworkController(QObject* parent)
	: NetworkControllerBase(parent)
{
}

void NetworkController::start(QSslSocket* socket)
{
	NetworkControllerBase::start(socket);
	while (true)
	{
		if (receiveBuffer.isEmpty())
		{
			if (socket->bytesAvailable() >= 8)
			{
				qDebug() << socket->bytesAvailable();
				receiveBuffer = socket->read(8);
			}
		}
		else
		{
			int size = *reinterpret_cast<const int*>(receiveBuffer.constData() + 4) & 0x00ffffff;
			if (socket->bytesAvailable() >= size - 8)
			{
				qDebug() << socket->bytesAvailable();
				receiveBuffer.append(socket->read(size - 8));
				auto request = Request::deserialize(receiveBuffer);
				auto response = handleRequest(request);
				socket->write(response->serialize());
				receiveBuffer.clear();
			}
		}
		QThread::msleep(10);
	}
}

QSharedPointer<Response> NetworkController::handleRequest(QSharedPointer<Request> request)
{
	switch (request->type)
	{
	case AppLayerMessage::Type::REGISTER:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<RegisterRequest*>(request.get())));
	case AppLayerMessage::Type::LOGIN:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<LoginRequest*>(request.get())));
	default:
		return nullptr;
	}
}

RegisterResponse* NetworkController::handleRequest(const RegisterRequest* request)
{
	return new RegisterResponse(request->id);
}

LoginResponse* NetworkController::handleRequest(const LoginRequest* request)
{
	return new LoginResponse(request->id);
}
