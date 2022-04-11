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
    auto server = new SslServer;
    this->server.reset(server);
    connect(server, &QTcpServer::newConnection, this, &MainState::newConnection);
    server->listen(QHostAddress::LocalHost, 8888);
    server->moveToThread(&serverThread);
    serverThread.start();
}

void MainState::onExit(QEvent* event)
{
}

void MainState::newConnection()
{
    while (auto socket = dynamic_cast<QSslSocket*>(server->nextPendingConnection()))
    {
    }
}
