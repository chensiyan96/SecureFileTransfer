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
	socket->moveToThread(&workerThread);
	this->socket.reset(socket);
}
