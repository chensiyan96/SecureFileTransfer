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
	logoutState = new LogoutState(&stateMachine);
	registerState = new RegisterState(&stateMachine);
	loginState = new LoginState(&stateMachine);
	mainState = new MainState(&stateMachine);

	disconnectedState->addTransition(ClientMainWindow::instance, &ClientMainWindow::connectToHostTriggered, connectingState);
	connectingState->addTransition(connectingState, &ConnectingState::canceled, disconnectedState);
	connectingState->addTransition(connectingState, &ConnectingState::connectFinished, loginState);
	logoutState->addTransition(ClientMainWindow::instance, &ClientMainWindow::registerTriggered, registerState);
	logoutState->addTransition(ClientMainWindow::instance, &ClientMainWindow::loginTriggered, loginState);
	logoutState->addTransition(ClientMainWindow::instance, &ClientMainWindow::disconnectFromHostTriggered, disconnectedState);
	registerState->addTransition(registerState, &RegisterState::canceled, logoutState);
	registerState->addTransition(registerState, &RegisterState::registerFinished, loginState);
	registerState->addTransition(ClientMainWindow::instance, &ClientMainWindow::disconnectFromHostTriggered, disconnectedState);
	loginState->addTransition(loginState, &LoginState::canceled, logoutState);
	loginState->addTransition(loginState, &LoginState::loginFinished, mainState);
	loginState->addTransition(ClientMainWindow::instance, &ClientMainWindow::disconnectFromHostTriggered, disconnectedState);
	mainState->addTransition(mainState, &MainState::logout, logoutState);
	mainState->addTransition(ClientMainWindow::instance, &ClientMainWindow::disconnectFromHostTriggered, disconnectedState);

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
	connectWidget->setAttribute(Qt::WA_DeleteOnClose, true);
	connectWidget->show();
	connect(connectWidget, &ConnectWidget::connectToHost, NetworkController::instance, &NetworkController::connectToHost);
	connect(connectWidget, &ConnectWidget::destroyed, this, &ConnectingState::onWidgetDestroyed);
	connect(NetworkController::instance, &NetworkController::succeeded, this, &ConnectingState::succeeded);
	connect(this, &ConnectingState::connectFinished, NetworkController::instance, &NetworkController::start);
}

void ConnectingState::onExit(QEvent* event)
{
	NetworkController::instance->disconnect(this);
	if (!connectWidget.isNull())
	{
		connectWidget->disconnect(this);
		connectWidget.reset(nullptr);
	}
}

void ConnectingState::onWidgetDestroyed()
{
	connectWidget.take();
	emit canceled();
}

void ConnectingState::succeeded()
{
	connectWidget->succeeded();
	emit connectFinished();
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
	registerWidget->setAttribute(Qt::WA_DeleteOnClose, true);
	registerWidget->show();
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &RegisterState::checkResponse);
	connect(registerWidget, &ConnectWidget::destroyed, this, &RegisterState::onWidgetDestroyed);
}

void RegisterState::onExit(QEvent* event)
{
	NetworkController::instance->disconnect(this);
	if (!registerWidget.isNull())
	{
		registerWidget->disconnect(this);
		registerWidget.reset(nullptr);
	}
}

void RegisterState::onWidgetDestroyed()
{
	registerWidget.take();
	emit canceled();
}

void RegisterState::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto registerResponse = dynamic_cast<RegisterResponse*>(response.get()))
	{
		switch (registerResponse->result)
		{
		case RegisterResponse::Result::SUCCESS:
			registerWidget->succeeded();
			emit registerFinished();
			break;
		case RegisterResponse::Result::INVALID_ARGUMENT:
			registerWidget->failed(u8"������Ч");
			break;
		case RegisterResponse::Result::REPEAT_USERNAME:
			registerWidget->failed(u8"�û����Ѿ�����");
			break;
		}
	}
}

void LoginState::onEntry(QEvent* event)
{
	auto loginWidget = new LoginWidget;
	this->loginWidget.reset(loginWidget);
	loginWidget->setAttribute(Qt::WA_QuitOnClose, false);
	loginWidget->setAttribute(Qt::WA_DeleteOnClose, true);
	loginWidget->show();
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &LoginState::checkResponse);
	connect(loginWidget, &ConnectWidget::destroyed, this, &LoginState::onWidgetDestroyed);
}

void LoginState::onExit(QEvent* event)
{
	NetworkController::instance->disconnect(this);
	if (!loginWidget.isNull())
	{
		loginWidget->disconnect(this);
		loginWidget.reset(nullptr);
	}
}

void LoginState::onWidgetDestroyed()
{
	loginWidget.take();
	emit canceled();
}

void LoginState::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto loginResponse = dynamic_cast<LoginResponse*>(response.get()))
	{
		switch (loginResponse->result)
		{
		case LoginResponse::Result::SUCCESS:
			loginWidget->succeeded();
			emit loginFinished();
			break;
		case LoginResponse::Result::INVALID_ARGUMENT:
			loginWidget->failed(u8"������Ч");
			break;
		case LoginResponse::Result::WRONG_USERNAME_OR_PASSWORD:
			loginWidget->failed(u8"�û������������");
			break;
		case LoginResponse::Result::LOGOUT_FIRST:
			loginWidget->failed(u8"���е�¼");
			break;
		}
	}
}

void MainState::onEntry(QEvent* event)
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, this, &MainState::checkResponse);

	auto request = NetworkController::instance->newRequest<ListFilesRequest>();
	ClientMainWindow::instance->onMainStateEntry();
}

void MainState::onExit(QEvent* event)
{
	ClientMainWindow::instance->disconnect(this);
	NetworkController::instance->disconnect(this);
	ClientMainWindow::instance->onMainStateExit();
}

void MainState::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	switch (response->type)
	{
	case AppLayerMessage::Type::LOGOUT:
		if (dynamic_cast<LogoutResponse*>(response.get())->result == LogoutResponse::Result::SUCCESS)
		{
			QMessageBox::information(nullptr, u8"��Ϣ", u8"ע���ɹ�", QMessageBox::Ok);
			emit logout();
		}
		break;
	case AppLayerMessage::Type::DELETE_USER:
		if (dynamic_cast<DeleteUserResponse*>(response.get())->result == DeleteUserResponse::Result::SUCCESS)
		{
			QMessageBox::information(nullptr, u8"��Ϣ", u8"ɾ���ɹ�", QMessageBox::Ok);
			emit logout();
		}
		break;
	}
}
