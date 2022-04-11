#include "stdafx.h"

#include "NetworkControllerBase.h"

NetworkControllerBase::NetworkControllerBase(QObject *parent)
	: QObject(parent)
{
	moveToThread(&workerThread);
	workerThread.start();
}

void NetworkControllerBase::start(QSslSocket* socket)
{
	this->socket = socket;
	socket->moveToThread(&workerThread);
}
