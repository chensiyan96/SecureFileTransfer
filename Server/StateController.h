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
	class MainState* mainState = nullptr;
};

class MainState : public QState
{
	Q_OBJECT

public:
	explicit MainState(QState* parent = nullptr) : QState(parent) {}

signals:
	void newSocket();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	void newConnection();

private:
	QScopedPointer<SslServer> server;
	QThread serverThread;
};
