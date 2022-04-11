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
};

class ConnectState : public QState
{
	Q_OBJECT

public:
	explicit ConnectState(QState* parent = nullptr) : QState(parent) {}

signals:
	void connectedToServer(QSslSocket* socket);

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
	
private:
	void connectToHost(QString host, quint16 port);
	void connectionSucceeded();

private:
	QScopedPointer<ConnectWidget> connectWidget;
	QScopedPointer<QSslSocket> socket;
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
	void registerRequest(QSharedPointer<RegisterRequest> request);
	void registerSucceeded();

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
	void loginRequest(QSharedPointer<LoginRequest> request);
	void loginSucceeded();

private:
	QScopedPointer<LoginWidget> loginWidget;
};
