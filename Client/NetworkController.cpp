#include "stdafx.h"

#include "NetworkController.h"

NetworkController* NetworkController::instance = nullptr;

NetworkController::NetworkController(QObject *parent)
	: QObject(parent)
{
	moveToThread(&workerThread);
	workerThread.start();
}
