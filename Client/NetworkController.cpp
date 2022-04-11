#include "stdafx.h"

#include "NetworkController.h"

NetworkController* NetworkController::instance = nullptr;

NetworkController::NetworkController(QObject *parent)
	: NetworkControllerBase(parent)
{
	sendQueue.resize(3);
}

void NetworkController::start(QSslSocket* socket)
{
	NetworkControllerBase::start(socket);
	while (true)
	{
		sendQueueMutex.lock();
		for (auto& queue : sendQueue)
		{
			if (!queue.empty())
			{
				auto request = queue.front();
				matchMap[request->id] = request;
				socket->write(request->serialize());
				queue.pop_front();
			}
		}
		sendQueueMutex.unlock();
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
		QThread::msleep(10);
	}
}

void NetworkController::sendRequest(QSharedPointer<Request> request, int priority)
{
	sendQueueMutex.lock();
	sendQueue[priority].push_back(request);
	sendQueueMutex.unlock();
}
