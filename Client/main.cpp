#include "stdafx.h"

#include "ClientMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = std::make_unique<ClientMainWindow>(new ClientMainWindow);
    w->show();
    w->startStateController();
    return a.exec();
}
