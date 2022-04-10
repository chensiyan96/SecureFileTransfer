#include "stdafx.h"

#include "ServerMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerMainWindow w;
    w.show();
    return a.exec();
}
