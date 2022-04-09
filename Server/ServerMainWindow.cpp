#include "stdafx.h"

#include "ServerMainWindow.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"
#include "../DatabaseAccess/DatabaseAccess.h"

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    auto db = new DatabaseAccess;
    auto test = new SecureFileTransfer;
    test->testServer();
}
