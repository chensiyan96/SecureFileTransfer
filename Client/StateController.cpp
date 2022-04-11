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

	connectState->addTransition(connectState, &ConnectState::connectedToServer, loginState);

	stateMachine.setInitialState(connectState);
	stateMachine.start();
}

void ConnectState::onEntry(QEvent* event)
{
	auto connectWidget = new ConnectWidget;
	this->connectWidget.reset(connectWidget);
	connectWidget->setAttribute(Qt::WA_QuitOnClose, false);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, this, &ConnectState::connectToHost);
	connect(this, &ConnectState::connectedToServer, NetworkController::instance, &NetworkController::start);

}

void ConnectState::onExit(QEvent* event)
{
	connectWidget.reset(nullptr);
	socket.reset(nullptr);
}

void ConnectState::connectToHost(QString host, quint16 port)
{
	auto socket = new QSslSocket;
	this->socket.reset(socket);
	socket->setPeerVerifyMode(QSslSocket::QueryPeer);
	connect(socket, &QSslSocket::encrypted, this, &ConnectState::connectionSucceeded);
	socket->connectToHostEncrypted(host, port);
}

void ConnectState::connectionSucceeded()
{
	connectWidget->connectionSucceeded();
	emit connectedToServer(this->socket.take());
}

void RegisterState::onEntry(QEvent* event)
{
	auto registerWidget = new RegisterWidget;
	this->registerWidget.reset(registerWidget);
	registerWidget->setAttribute(Qt::WA_QuitOnClose, false);
	registerWidget->show();
	connect(registerWidget, &RegisterWidget::registerRequest, this, &RegisterState::registerRequest);
}

void RegisterState::onExit(QEvent* event)
{
	registerWidget.reset(nullptr);
}

void RegisterState::registerRequest(QSharedPointer<RegisterRequest> request)
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &RegisterState::registerSucceeded);
	NetworkController::instance->sendRequest(request, 0);
}

void RegisterState::registerSucceeded()
{
	registerWidget->registerSucceeded();
	emit registerFinished();
}

void LoginState::onEntry(QEvent* event)
{
	auto loginWidget = new LoginWidget;
	this->loginWidget.reset(loginWidget);
	loginWidget->setAttribute(Qt::WA_QuitOnClose, false);
	loginWidget->show();
	connect(loginWidget, &LoginWidget::loginRequest, this, &LoginState::loginRequest);
}

void LoginState::onExit(QEvent* event)
{
	loginWidget.reset(nullptr);
}

void LoginState::loginRequest(QSharedPointer<LoginRequest> request)
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &LoginState::loginSucceeded);
	NetworkController::instance->sendRequest(request, 0);
}

void LoginState::loginSucceeded()
{
	loginWidget->loginSucceeded();
	emit loginFinished();
}
