#include "stdafx.h"

#include "ClientMainWindow.h"
#include "StateController.h"
#include "NetworkController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = std::make_unique<ClientMainWindow>(new ClientMainWindow);
    w->show();
    StateController::instance = new StateController;
    NetworkController::instance = new NetworkController;
    return a.exec();
}
