#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"
#include "ServerWidget.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
    mainState = new MainState(&stateMachine);

    stateMachine.addState(mainState);

    stateMachine.setInitialState(mainState);
    stateMachine.start();
}

void SetupState::onEntry(QEvent* event)
{
}

void SetupState::onExit(QEvent* event)
{
}

MainState::~MainState()
{
    if (serverThread.isRunning())
    {
        serverThread.quit();
        serverThread.wait();
    }
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
    serverThread.quit();
    serverThread.wait();
    server.reset(nullptr);
}

void MainState::newConnection()
{
    while (auto socket = dynamic_cast<QSslSocket*>(server->nextPendingConnection()))
    {
    }
}
