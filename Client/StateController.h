#pragma once

#include "ConnectWidget.h"
#include "RegisterWidget.h"
#include "LoginWidget.h"

class StateController : public QObject
{
	Q_OBJECT

public:
	static StateController* instance;

public:
	explicit StateController(QObject* parent = nullptr);

private:
	QStateMachine stateMachine;
	class DisconnectedState* disconnectedState = nullptr;
	class ConnectingState* connectingState = nullptr;
	class RegisterState* registerState = nullptr;
	class LoginState* loginState = nullptr;
	class MainState* mainState = nullptr;
};

class DisconnectedState : public QState
{
	Q_OBJECT

public:
	explicit DisconnectedState(QState* parent = nullptr) : QState(parent) {}

signals:
	void connectTriggered();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
};

class ConnectingState : public QState
{
	Q_OBJECT

public:
	explicit ConnectingState(QState* parent = nullptr) : QState(parent) {}

signals:
	void canceled();
	void connectedToServer();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
	
private:
	void connectionSucceeded();

private:
	QScopedPointer<ConnectWidget> connectWidget;
};

class LogoutState : public QState
{
	Q_OBJECT

public:
	explicit LogoutState(QState* parent = nullptr) : QState(parent) {}

signals:
	void registerTriggered();
	void loginTriggered();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
};

class RegisterState : public QState
{
	Q_OBJECT

public:
	explicit RegisterState(QState* parent = nullptr) : QState(parent) {}

signals:
	void registerFinished();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	void checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);

private:
	QScopedPointer<RegisterWidget> registerWidget;
};

class LoginState : public QState
{
	Q_OBJECT

public:
	explicit LoginState(QState* parent = nullptr) : QState(parent) {}

signals:
	void loginFinished();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	void checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);

private:
	QScopedPointer<LoginWidget> loginWidget;
};

class MainState : public QState
{
	Q_OBJECT

public:
	explicit MainState(QState* parent = nullptr) : QState(parent) {}

signals:
	void newSocket(QSslSocket* socket);

private:
	void sendRequest(QSharedPointer<Request> request, int priority);

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
};
