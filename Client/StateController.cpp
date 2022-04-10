#include "stdafx.h"

#include "StateController.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
	connectState = new ConnectState(&stateMachine);
	registerState = new RegisterState(&stateMachine);
	loginState = new LoginState(&stateMachine);

	stateMachine.addState(connectState);
	stateMachine.addState(registerState);
	stateMachine.addState(loginState);

	connectState->addTransition(this, &StateController::connectedToServer, loginState);

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

void StateController::registerRequest(RegisterRequest* request)
{
	socket->write(request->serialize());
}

void StateController::loginRequest(LoginRequest* request)
{
	socket->write(request->serialize());
}

void StateController::encryptionSucceeded()
{
	connectState->encryptionSucceeded();
	emit connectedToServer(this->socket.get());
}

void ConnectState::encryptionSucceeded()
{
	connectWidget->encryptionSucceeded();
}

void ConnectState::onEntry(QEvent* event)
{
	auto connectWidget = new ConnectWidget;
	this->connectWidget.reset(connectWidget);
	connectWidget->setAttribute(Qt::WA_QuitOnClose, false);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, StateController::instance, &StateController::connectToHost);
}

void ConnectState::onExit(QEvent* event)
{
	connectWidget = nullptr;
}

void RegisterState::registerSucceeded()
{
	registerWidget->registerSucceeded();
}

void RegisterState::onEntry(QEvent* event)
{
	auto registerWidget = new RegisterWidget;
	this->registerWidget.reset(registerWidget);
	registerWidget->setAttribute(Qt::WA_QuitOnClose, false);
	registerWidget->show();
	connect(registerWidget, &RegisterWidget::registerRequest, StateController::instance, &StateController::registerRequest);
}

void RegisterState::onExit(QEvent* event)
{
	registerWidget = nullptr;
}

void LoginState::loginSucceeded()
{
	loginWidget->loginSucceeded();
}

void LoginState::onEntry(QEvent* event)
{
	auto loginWidget = new LoginWidget;
	this->loginWidget.reset(loginWidget);
	loginWidget->setAttribute(Qt::WA_QuitOnClose, false);
	loginWidget->show();
	connect(loginWidget, &LoginWidget::loginRequest, StateController::instance, &StateController::loginRequest);
}

void LoginState::onExit(QEvent* event)
{
	loginWidget = nullptr;
}
