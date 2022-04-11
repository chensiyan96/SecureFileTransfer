#include "stdafx.h"

#include "ClientMainWindow.h"
#include "StateController.h"
#include "NetworkController.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    auto w = std::make_unique<ClientMainWindow>(new ClientMainWindow);
    w->show();
    StateController::instance = new StateController;
    NetworkController::instance = new NetworkController;
    QObject::connect(StateController::instance, &StateController::connectedToServer, NetworkController::instance, &NetworkController::start);
    return a.exec();
}
