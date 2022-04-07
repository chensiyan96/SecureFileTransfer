#include "stdafx.h"

#include "ClientMainWindow.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    auto test = new SecureFileTransfer;
    test->testClient();
}
