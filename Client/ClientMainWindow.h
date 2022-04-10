#pragma once

#include "ui_ClientMainWindow.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(QWidget *parent = nullptr);

private:
    Ui::ClientMainWindowClass ui;
};
