#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"
#include "ClientMainWindow.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
	connectState = new ConnectState(&stateMachine);
	registerState = new RegisterState(&stateMachine);
	loginState = new LoginState(&stateMachine);
	mainState = new MainState(&stateMachine);

	stateMachine.addState(connectState);
	stateMachine.addState(registerState);
	stateMachine.addState(loginState);
	stateMachine.addState(mainState);

	connectState->addTransition(connectState, &ConnectState::connectedToServer, loginState);
	loginState->addTransition(loginState, &LoginState::loginFinished, mainState);

	stateMachine.setInitialState(connectState);
	stateMachine.start();
}

void ConnectState::onEntry(QEvent* event)
{
	auto connectWidget = new ConnectWidget;
	this->connectWidget.reset(connectWidget);
	connectWidget->setAttribute(Qt::WA_QuitOnClose, false);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, NetworkController::instance, &NetworkController::connectToHost);
	connect(NetworkController::instance, &NetworkController::connectionSucceeded, this, &ConnectState::connectionSucceeded);
	connect(this, &ConnectState::connectedToServer, NetworkController::instance, &NetworkController::start);
}

void ConnectState::onExit(QEvent* event)
{
	connectWidget.reset(nullptr);
	NetworkController::instance->disconnect(this);
}

void ConnectState::connectionSucceeded()
{
	connectWidget->connectionSucceeded();
	emit connectedToServer();
}

void RegisterState::onEntry(QEvent* event)
{
	auto registerWidget = new RegisterWidget;
	this->registerWidget.reset(registerWidget);
	registerWidget->setAttribute(Qt::WA_QuitOnClose, false);
	registerWidget->show();
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &RegisterState::checkResponse);
	connect(registerWidget, &RegisterWidget::sendRequest,
		[](QSharedPointer<Request> request, int priority) {
			NetworkController::instance->sendRequest(request, priority);
		});
}

void RegisterState::onExit(QEvent* event)
{
	registerWidget.reset(nullptr);
	NetworkController::instance->disconnect(this);
}

void RegisterState::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto registerResponse = dynamic_cast<RegisterResponse*>(response.get()))
	{
		if (registerResponse->result == RegisterResponse::Result::SUCCESS)
		{
			registerWidget->registerSucceeded();
			emit registerFinished();
		}
	}
}

void LoginState::onEntry(QEvent* event)
{
	auto loginWidget = new LoginWidget;
	this->loginWidget.reset(loginWidget);
	loginWidget->setAttribute(Qt::WA_QuitOnClose, false);
	loginWidget->show();
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &LoginState::checkResponse);
	connect(loginWidget, &LoginWidget::sendRequest,
		[](QSharedPointer<Request> request, int priority) {
			NetworkController::instance->sendRequest(request, priority);
		});
}

void LoginState::onExit(QEvent* event)
{
	loginWidget.reset(nullptr);
	NetworkController::instance->disconnect(this);
}

void LoginState::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto loginResponse = dynamic_cast<LoginResponse*>(response.get()))
	{
		if (loginResponse->result == LoginResponse::Result::SUCCESS)
		{
			loginWidget->loginSucceeded();
			emit loginFinished();
		}
	}
}

void MainState::onEntry(QEvent* event)
{
	connect(ClientMainWindow::instance, &ClientMainWindow::sendRequest,
		[](QSharedPointer<Request> request, int priority) {
			NetworkController::instance->sendRequest(request, priority);
		});
}

void MainState::onExit(QEvent* event)
{
	NetworkController::instance->disconnect(this);
}
