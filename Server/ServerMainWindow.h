#pragma once

#include "ui_ServerMainWindow.h"

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerMainWindow(QWidget *parent = nullptr);

private:
    Ui::ServerMainWindowClass ui;
};
