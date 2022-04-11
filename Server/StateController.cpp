#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
    mainState = new MainState(&stateMachine);

    stateMachine.addState(mainState);

    stateMachine.setInitialState(mainState);
    stateMachine.start();
}

void MainState::onEntry(QEvent* event)
{
    auto sslServer = new SslServer;
    this->sslServer.reset(sslServer);
    connect(sslServer, &QTcpServer::newConnection, this, &MainState::newConnection);
    sslServer->listen(QHostAddress::LocalHost, 8888);
}

void MainState::onExit(QEvent* event)
{
}

void MainState::newConnection()
{
    while (auto socket = dynamic_cast<QSslSocket*>(sslServer->nextPendingConnection()))
    {
        auto newNetworkController = new NetworkController;
        connect(this, &MainState::newSocket, newNetworkController, &NetworkController::start);
        emit newSocket(socket);
        //allNetworkControllers.insert(QScopedPointer<NetworkController>(newNetworkController));
    }
}
