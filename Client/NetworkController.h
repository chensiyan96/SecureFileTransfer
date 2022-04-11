#pragma once

#include "../SecureFileTransfer/NetworkControllerBase.h"

class NetworkController : public NetworkControllerBase
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

	void start(QSslSocket* socket); // slot
	void sendRequest(QSharedPointer<Request> request, int priority);

signals:
	void receivedResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);

private:
	QVector<QQueue<QSharedPointer<Request>>> sendQueue;
	QMutex sendQueueMutex;
	QMap<quint32, QSharedPointer<Request>> matchMap;
	QByteArray receiveBuffer;
	quint32 nextRequestId = 1;
};
