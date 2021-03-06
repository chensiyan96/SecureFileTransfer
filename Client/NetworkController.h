#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"

class NetworkController : public QObject
{
	Q_OBJECT

public:
	static NetworkController* instance;

public:
	explicit NetworkController(QObject* parent = nullptr);

	template<class Request>
	QSharedPointer<Request> newRequest()
	{
		return QSharedPointer<Request>(new Request(nextRequestId++));
	}

	void connectToHost(QString host, quint16 port);
	void disconnectFromHost();

	void start(); // slot
	void sendRequest(QSharedPointer<Request> request, int priority);

signals:
	void requestAdded();
	void succeeded();
	void receivedResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);
	void disconnected();

private:
	void writeRequest();
	void readResponse();
	void cleanUp();

private:
	QTimer timer;
	QScopedPointer<QSslSocket> socket;
	QVector<QQueue<QSharedPointer<Request>>> sendQueue;
	QMutex sendQueueMutex;
	QMap<quint32, QSharedPointer<Request>> matchMap;
	QByteArray receiveBuffer;
	QByteArray sendBuffer;
	quint32 sendOffset = 0;
	std::atomic<quint32> nextRequestId = 1;
};
