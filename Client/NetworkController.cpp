#include "stdafx.h"

#include "NetworkController.h"

NetworkController* NetworkController::instance = nullptr;

NetworkController::NetworkController(QObject *parent)
	: QObject(parent)
{
	sendQueue.resize(3);
	timer.start(10);
}

void NetworkController::connectToHost(QString host, quint16 port)
{
	this->socket.reset(new QSslSocket);
	socket->setPeerVerifyMode(QSslSocket::QueryPeer);
	connect(socket.get(), &QSslSocket::encrypted, this, &NetworkController::connectionSucceeded);
	socket->connectToHostEncrypted(host, port);
}

void NetworkController::start()
{
	connect(&timer, &QTimer::timeout, this, &NetworkController::writeRequest);
	connect(&timer, &QTimer::timeout, this, &NetworkController::readResponse);
	connect(socket.get(), &QSslSocket::readyRead, this, &NetworkController::readResponse);
}

void NetworkController::sendRequest(QSharedPointer<Request> request, int priority)
{
	sendQueueMutex.lock();
	sendQueue[priority].push_back(request);
	sendQueueMutex.unlock();
}

void NetworkController::writeRequest()
{
	sendQueueMutex.lock();
	for (auto& queue : sendQueue)
	{
		if (!queue.empty())
		{
			auto request = queue.front();
			matchMap[request->id] = request;
			auto bytes = request->serialize();
			auto n = socket->write(bytes);
			assert(n == bytes.size());
			queue.pop_front();
		}
	}
	sendQueueMutex.unlock();
}

void NetworkController::readResponse()
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
			auto response = Response::deserialize(receiveBuffer);
			auto request = matchMap[response->id];
			if (!request.isNull())
			{
				emit receivedResponse(request, response);
			}
			matchMap.remove(response->id);
			receiveBuffer.clear();
		}
	}
}
