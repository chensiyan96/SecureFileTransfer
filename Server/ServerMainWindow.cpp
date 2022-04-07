#include "stdafx.h"

#include "ServerMainWindow.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    auto test = new SecureFileTransfer;
    test->testServer();
}
