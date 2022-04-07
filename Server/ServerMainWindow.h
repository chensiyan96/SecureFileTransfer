#pragma once

#include "ui_ServerMainWindow.h"

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::ServerMainWindowClass ui;
};
