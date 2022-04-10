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

	void connectToHost(QString host, quint16 port);
	void registerRequest(RegisterRequest* request);
	void loginRequest(LoginRequest* request);

signals:
	void connectedToServer(QSslSocket* socket);

private:
	void encryptionSucceeded();

private:
	QStateMachine stateMachine;
	std::unique_ptr<QSslSocket> socket = nullptr;
	class ConnectState* connectState = nullptr;
	class RegisterState* registerState = nullptr;
	class LoginState* loginState = nullptr;
};

class ConnectState : public QState
{
	Q_OBJECT

public:
	explicit ConnectState(QState* parent = nullptr) : QState(parent) {}

public:
	void encryptionSucceeded();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
	
private:
	std::unique_ptr<ConnectWidget> connectWidget = nullptr;
};

class RegisterState : public QState
{
	Q_OBJECT

public:
	explicit RegisterState(QState* parent = nullptr) : QState(parent) {}

	void registerSucceeded();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	std::unique_ptr<RegisterWidget> registerWidget = nullptr;
};

class LoginState : public QState
{
	Q_OBJECT

public:
	explicit LoginState(QState* parent = nullptr) : QState(parent) {}

	void loginSucceeded();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	std::unique_ptr<LoginWidget> loginWidget = nullptr;
};
