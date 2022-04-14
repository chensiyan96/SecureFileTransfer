#include "stdafx.h"

#include "NetworkController.h"

NetworkController* NetworkController::instance = nullptr;

NetworkController::NetworkController(QObject *parent)
	: QObject(parent)
{
	sendQueue.resize(3);
	timer.start(100);
}

void NetworkController::connectToHost(QString host, quint16 port)
{
	this->socket.reset(new QSslSocket);
	socket->setPeerVerifyMode(QSslSocket::QueryPeer);
	connect(socket.get(), &QSslSocket::encrypted, this, &NetworkController::succeeded);
	connect(socket.get(), &QSslSocket::disconnected, this, &NetworkController::disconnected);
	connect(socket.get(), &QSslSocket::disconnected, this, &NetworkController::cleanUp);
	socket->connectToHostEncrypted(host, port);
}

void NetworkController::disconnectFromHost()
{
	socket->disconnectFromHost();
	socket->waitForDisconnected();
	socket.reset(nullptr);
	QMessageBox::information(nullptr, u8"消息", u8"连接已断开", QMessageBox::Ok);
}

void NetworkController::start()
{
	connect(&timer, &QTimer::timeout, this, &NetworkController::writeRequest);
	connect(&timer, &QTimer::timeout, this, &NetworkController::readResponse);
	connect(this, &NetworkController::requestAdded, this, &NetworkController::writeRequest);
	connect(socket.get(), &QSslSocket::encryptedBytesWritten, this, &NetworkController::writeRequest);
	connect(socket.get(), &QSslSocket::readyRead, this, &NetworkController::readResponse);
}

void NetworkController::sendRequest(QSharedPointer<Request> request, int priority)
{
	if (socket.isNull())
	{
		return;
	}
	sendQueueMutex.lock();
	sendQueue[priority].push_back(request);
	sendQueueMutex.unlock();
	emit requestAdded();
}

void NetworkController::writeRequest()
{
	if (sendBuffer.isEmpty())
	{
		sendQueueMutex.lock();
		for (auto& queue : sendQueue)
		{
			if (!queue.isEmpty())
			{
				auto request = queue.front();
				matchMap[request->id] = request;
				auto bytes = request->serialize();
				sendOffset = quint32(socket->write(bytes));
				queue.pop_front();
				if (sendOffset < quint32(bytes.size()))
				{
					sendBuffer = bytes;
					break; // 发送缓冲区满，暂停发送
				}
				if (sendQueue[0].isEmpty() && sendQueue[1].isEmpty())
				{
					// 优先级为2的队列是文件内容，比较长，容易阻塞高优先级报文，不着急发送
					break;
				}
			}
		}
		sendQueueMutex.unlock();
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
	if (socket->encryptedBytesToWrite() == 0)
	{
		readResponse();
	}
}

void NetworkController::readResponse()
{
	while (socket->encryptedBytesToWrite() == 0)
	{
		if (receiveBuffer.isEmpty())
		{
			if (socket->bytesAvailable() >= 8)
			{
				// qDebug() << socket->bytesAvailable();
				receiveBuffer = socket->read(8);
				if (socket->bytesAvailable() > 0)
				{
					goto READ_BODY;
				}
			}
			else
			{
				return;
			}
		}
		else
		{
		READ_BODY:
			int size = *reinterpret_cast<const int*>(receiveBuffer.constData() + 4) & 0x00ffffff;
			if (socket->bytesAvailable() >= size - 8)
			{
				// qDebug() << socket->bytesAvailable();
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
			else
			{
				return;
			}
		}
	}
}

void NetworkController::cleanUp()
{
	for (auto& queue : sendQueue)
	{
		queue.clear();
	}

}
