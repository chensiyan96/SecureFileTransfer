#pragma once

class NetworkController : public QObject
{
	Q_OBJECT

public:
	static NetworkController* instance;

public:
	explicit NetworkController(QObject* parent = nullptr);

	template<class Request>
	Request* newRequest() { return new Request(nextRequestId++); }

private:
	QThread workerThread;
	QSslSocket* socket = nullptr;
	quint32 nextRequestId = 1;
};
