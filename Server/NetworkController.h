#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"

class NetworkController : public QObject
{
	Q_OBJECT

public:
	explicit NetworkController(QSslSocket* socket, QObject* parent = nullptr);

	void start(); // slot

private:
	void readRequest();
	void writeResponse();
	void sendResponse(QSharedPointer<Response> response);

private:
	QTimer timer;
	QScopedPointer<QSslSocket> socket;
	QQueue<QSharedPointer<Response>> sendQueue;
	QByteArray receiveBuffer;
	QByteArray sendBuffer;
	quint32 sendOffset = 0;
};
