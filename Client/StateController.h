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
	class ConnectState* connectState = nullptr;
	class RegisterState* registerState = nullptr;
	class LoginState* loginState = nullptr;
	class MainState* mainState = nullptr;
};

class ConnectState : public QState
{
	Q_OBJECT

public:
	explicit ConnectState(QState* parent = nullptr) : QState(parent) {}

signals:
	void connectedToServer();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
	
private:
	void connectionSucceeded();

private:
	QScopedPointer<ConnectWidget> connectWidget;
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

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
};
