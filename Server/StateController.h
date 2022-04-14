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

public:
	QSslCertificate certificate;
	QSslKey privateKey;

private:
	QStateMachine stateMachine;
	class SetupState* setupState = nullptr;
	class MainState* mainState = nullptr;
};

class SetupState : public QState
{
	Q_OBJECT

public:
	explicit SetupState(QState* parent = nullptr) : QState(parent) {}

signals:
	void setupFinished();

private:
	void setCertificate(QString path);
	void setPrivateKey(QString path);
	void setDatabase(QString path);
	void setup();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;

private:
	bool databaseInit = false;
};

class MainState : public QState
{
	Q_OBJECT

public:
	explicit MainState(QState* parent = nullptr) : QState(parent) {}
	~MainState();

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
