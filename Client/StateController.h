#pragma once

#include "ConnectWidget.h"
#include "RegisterWidget.h"

class StateController : public QObject
{
	Q_OBJECT

public:
	explicit StateController(QObject* parent = nullptr);

	void startStateMachine();
	void connectToHost(QString host, quint16 port);

signals:
	void connectedToServer(QSslSocket* socket);

private:
	void encryptionSucceeded();

private:
	QStateMachine stateMachine;
	std::unique_ptr<QSslSocket> socket = nullptr;
	class ConnectState* connectState = nullptr;
};

class ConnectState : public QState
{
	Q_OBJECT

public:
	ConnectState(StateController* stateController, QState* parent = nullptr)
		: QState(parent), stateController(stateController) {}

public:
	void encryptionSucceeded();

protected:
	void onEntry(QEvent* event) override;
	void onExit(QEvent* event) override;
	
private:
	StateController* stateController;
	std::unique_ptr<ConnectWidget> connectWidget = nullptr;
};
