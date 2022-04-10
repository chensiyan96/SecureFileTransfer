#include "stdafx.h"

#include "StateController.h"

StateController::StateController(QObject* parent)
	: QObject(parent)
{
	connectState = new ConnectState(this, &stateMachine);

	stateMachine.addState(connectState);
}

void StateController::startStateMachine()
{
	stateMachine.setInitialState(connectState);
	stateMachine.start();
}

void StateController::connectToHost(QString host, quint16 port)
{
	auto socket = new QSslSocket;
	this->socket.reset(socket);
	socket->setPeerVerifyMode(QSslSocket::QueryPeer);
	connect(socket, &QSslSocket::encrypted, this, &StateController::encryptionSucceeded);
	socket->connectToHostEncrypted(host, port);
}

void StateController::encryptionSucceeded()
{
	connectState->encryptionSucceeded();
	emit connectedToServer(this->socket.get());
}

void ConnectState::encryptionSucceeded()
{
	connectWidget->encryptionSucceeded();
	QMessageBox::information(connectWidget.get(), u8"消息", u8"建立加密连接成功", QMessageBox::Ok);
	connectWidget = nullptr;
}

void ConnectState::onEntry(QEvent* event)
{
	auto connectWidget = new ConnectWidget;
	this->connectWidget.reset(connectWidget);
	connectWidget->setAttribute(Qt::WA_QuitOnClose, false);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, stateController, &StateController::connectToHost);
}

void ConnectState::onExit(QEvent* event)
{
	connectWidget = nullptr;
}
