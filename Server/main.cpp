#include "stdafx.h"

#include "ServerMainWindow.h"
#include "StateController.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    QScopedPointer<ServerMainWindow> w(new ServerMainWindow);
    w->show();
    StateController::instance = new StateController;

    auto ret = a.exec();
    delete StateController::instance;
    return ret;
}
