#include "stdafx.h"

#include "SslServer.h"
#include "StateController.h"

SslServer::SslServer(QObject* parent)
    : QTcpServer(parent)
{
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    auto socket = new QSslSocket;
    socket->setLocalCertificate(StateController::instance->certificate);
    socket->setPrivateKey(StateController::instance->privateKey);
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        auto newNetworkController = new NetworkController(socket);
        connect(this, &SslServer::newConnection, newNetworkController, &NetworkController::start);
        addPendingConnection(socket);
        allNetworkControllers.emplace(newNetworkController);
    }
    else {
        delete socket;
    }
}
