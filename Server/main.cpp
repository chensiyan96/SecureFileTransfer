#include "stdafx.h"

#include "ServerWidget.h"
#include "StateController.h"
#include "RequestController.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    QScopedPointer<ServerWidget> w(new ServerWidget);
    w->show();
    StateController::instance = new StateController;
    RequestController::instance = new RequestController;

    auto ret = a.exec();
    delete StateController::instance;
    return ret;
}
