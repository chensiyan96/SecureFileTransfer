#pragma once

#include "NetworkController.h"
#include "SslServer.h"

class StateController : public QObject
{
	Q_OBJECT

public:
	static StateController* instance;

public:
	explicit StateController(QObject* parent = nullptr);

private:
	QStateMachine stateMachine;
	QScopedPointer<QSslSocket> socket;
	class MainState* mainState;
};

class MainState : public QState
{
	Q_OBJECT

public:
	explicit MainState(QState* parent = nullptr) : QState(parent) {}

signals:
	void newSocket(QSslSocket* socket); // slot

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	void newConnection();

private:
	QScopedPointer<SslServer> sslServer;
	//QSet<QScopedPointer<NetworkController>> allNetworkControllers;
};
