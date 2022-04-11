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
	class MainState* mainState;
};

inline bool operator==(const QScopedPointer<NetworkController>& a, const QScopedPointer<NetworkController>& b)
{
	return reinterpret_cast<size_t>(a.get()) == reinterpret_cast<size_t>(b.get());
}

inline uint qHash(const QScopedPointer<NetworkController>& a)
{
	return qHash(reinterpret_cast<size_t>(a.get()));
}

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
	QScopedPointer<SslServer> server;
	QSet<QScopedPointer<NetworkController>> allNetworkControllers;
};
