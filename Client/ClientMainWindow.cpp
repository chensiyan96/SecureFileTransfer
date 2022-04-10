#include "stdafx.h"

#include "ClientMainWindow.h"
#include "RegisterWidget.h"
#include "ConnectWidget.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}
