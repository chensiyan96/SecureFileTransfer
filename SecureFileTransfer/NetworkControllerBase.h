#pragma once

#include "AppLayerMessage.h"

class SECUREFILETRANSFER_EXPORT NetworkControllerBase : public QObject
{
	Q_OBJECT

protected:
	explicit NetworkControllerBase(QObject* parent);

	void start(QSslSocket* socket);

protected:
	QThread workerThread;
	QScopedPointer<QSslSocket> socket;
};
