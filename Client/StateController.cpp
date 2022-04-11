#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"

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

void StateController::registerRequest(QSharedPointer<RegisterRequest> request)
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &StateController::registerSucceeded);
	NetworkController::instance->sendRequest(request, 0);
}

void StateController::loginRequest(QSharedPointer<LoginRequest> request)
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &StateController::loginSucceeded);
	NetworkController::instance->sendRequest(request, 0);
}

void StateController::encryptionSucceeded()
{
	connectState->encryptionSucceeded();
	emit connectedToServer(this->socket.get());
}

void StateController::registerSucceeded()
{
	registerState->registerSucceeded();
}

void StateController::loginSucceeded()
{
	loginState->loginSucceeded();
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
	connectWidget.reset(nullptr);
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
	registerWidget.reset(nullptr);
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
	loginWidget.reset(nullptr);
}
