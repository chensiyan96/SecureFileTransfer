#pragma once

#include "ui_ClientMainWindow.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientMainWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::ClientMainWindowClass ui;
};
