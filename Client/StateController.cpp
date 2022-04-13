#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"
#include "ClientMainWindow.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
	disconnectedState = new DisconnectedState(&stateMachine);
	connectingState = new ConnectingState(&stateMachine);
	registerState = new RegisterState(&stateMachine);
	loginState = new LoginState(&stateMachine);
	mainState = new MainState(&stateMachine);
	
	stateMachine.addState(disconnectedState);
	stateMachine.addState(connectingState);
	stateMachine.addState(registerState);
	stateMachine.addState(loginState);
	stateMachine.addState(mainState);

	disconnectedState->addTransition(ClientMainWindow::instance, &ClientMainWindow::connectToHostTriggered, connectingState);
	connectingState->addTransition(connectingState, &ConnectingState::canceled, disconnectedState);
	connectingState->addTransition(connectingState, &ConnectingState::connectedToServer, loginState);
	registerState->addTransition(registerState, &RegisterState::registerFinished, loginState);
	loginState->addTransition(loginState, &LoginState::loginFinished, mainState);

	stateMachine.setInitialState(connectingState);
	stateMachine.start();
}

void DisconnectedState::onEntry(QEvent* event)
{
}

void DisconnectedState::onExit(QEvent* event)
{
}

void ConnectingState::onEntry(QEvent* event)
{
	auto connectWidget = new ConnectWidget;
	this->connectWidget.reset(connectWidget);
	connectWidget->setAttribute(Qt::WA_QuitOnClose, false);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, NetworkController::instance, &NetworkController::connectToHost);
	//connect(connectWidget, &ConnectWidget::canceled, this, &ConnectingState::canceled);
	connect(NetworkController::instance, &NetworkController::connectionSucceeded, this, &ConnectingState::connectionSucceeded);
	connect(this, &ConnectingState::connectedToServer, NetworkController::instance, &NetworkController::start);
}

void ConnectingState::onExit(QEvent* event)
{
	NetworkController::instance->disconnect(this);
	connectWidget->disconnect(this);
	connectWidget.reset(nullptr);
}

void ConnectingState::connectionSucceeded()
{
	connectWidget->connectionSucceeded();
	emit connectedToServer();
}

void LogoutState::onEntry(QEvent* event)
{
}

void LogoutState::onExit(QEvent* event)
{
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
	connect(ClientMainWindow::instance, &ClientMainWindow::sendRequest, this, &MainState::sendRequest);
	auto request = NetworkController::instance->newRequest<ListFilesRequest>();
	request->directory = "D:/SecureFileTransfer";
	NetworkController::instance->sendRequest(request, 0);
}

void MainState::onExit(QEvent* event)
{
	ClientMainWindow::instance->disconnect(this);
	NetworkController::instance->disconnect(this);
}

void MainState::sendRequest(QSharedPointer<Request> request, int priority)
{
	NetworkController::instance->sendRequest(request, priority);
}
