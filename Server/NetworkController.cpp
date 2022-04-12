#include "stdafx.h"

#include "NetworkController.h"
#include "RequestController.h"

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
	connect(socket.get(), &QSslSocket::encryptedBytesWritten, this, &NetworkController::writeResponse);
	connect(socket.get(), &QSslSocket::readyRead, this, &NetworkController::readRequest);
	connect(RequestController::instance, &RequestController::sendResponse, this, &NetworkController::sendResponse);
}

void NetworkController::readRequest()
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
			RequestController::instance->handleRequestAsync(request);
			receiveBuffer.clear();
		}
	}
}

void NetworkController::writeResponse()
{
	if (sendBuffer.isEmpty())
	{
		if (!sendQueue.isEmpty())
		{
			auto bytes = sendQueue.front()->serialize();
			sendOffset = quint32(socket->write(bytes));
			sendQueue.pop_front();
			if (sendOffset < quint32(bytes.size()))
			{
				sendBuffer = bytes;
				return; // ·¢ËÍ»º³åÇøÂú£¬ÔÝÍ£·¢ËÍ
			}
		}
	}
	else
	{
		auto size = quint32(sendBuffer.size());
		sendOffset += quint32(socket->write(sendBuffer.constData() + sendOffset, size - sendOffset));
		if (sendOffset >= size)
		{
			assert(sendOffset == size);
			sendBuffer.clear();
			sendOffset = 0;
		}
	}
}

void NetworkController::sendResponse(QSharedPointer<Response> response)
{
	sendQueue.push_back(response);
	writeResponse();
}
