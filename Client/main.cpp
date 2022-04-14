#include "stdafx.h"

#include "ClientMainWindow.h"
#include "StateController.h"
#include "NetworkController.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    NetworkController::instance = new NetworkController;
    ClientMainWindow::instance = new ClientMainWindow;
    ClientMainWindow::instance->show();
    StateController::instance = new StateController;

    QThread networkThread;
    NetworkController::instance->moveToThread(&networkThread);
    networkThread.start();

    auto ret = a.exec();
    QObject::connect(&networkThread, &QThread::finished, NetworkController::instance, &QObject::deleteLater);
    networkThread.quit();
    delete StateController::instance;
    delete ClientMainWindow::instance;
    networkThread.wait();
    return ret;
}
