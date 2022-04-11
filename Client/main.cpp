#include "stdafx.h"

#include "ClientMainWindow.h"
#include "StateController.h"
#include "NetworkController.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    QScopedPointer<ClientMainWindow> w(new ClientMainWindow);
    w->show();
    StateController::instance = new StateController;
    NetworkController::instance = new NetworkController;
    QThread networkThread;
    NetworkController::instance->moveToThread(&networkThread);
    networkThread.start();
    return a.exec();
}
