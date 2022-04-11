#include "stdafx.h"

#include "NetworkController.h"

NetworkController::NetworkController(QSslSocket* socket, QObject* parent)
	: QObject(parent), socket(socket)
{
	timer.start(100);
}

void NetworkController::start()
{
	socket->startServerEncryption();
	socket->waitForEncrypted();
	connect(&timer, &QTimer::timeout, this, &NetworkController::writeResponse);
	connect(&timer, &QTimer::timeout, this, &NetworkController::readRequest);
	connect(socket.get(), &QSslSocket::readyRead, this, &NetworkController::readRequest);
	return;
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
				auto bytes = response->serialize();
				auto n = socket->write(bytes);
				assert(n == bytes.size());
				socket->flush();
				receiveBuffer.clear();
			}
		}
		QThread::msleep(10);
	}
}

void NetworkController::readRequest()
{
	//socket->waitForReadyRead();
	//qDebug() << socket->bytesAvailable();
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
			auto bytes = response->serialize();
			auto n = socket->write(bytes);
			assert(n == bytes.size());
			socket->flush();
			receiveBuffer.clear();
		}
	}
}

void NetworkController::writeResponse()
{
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
