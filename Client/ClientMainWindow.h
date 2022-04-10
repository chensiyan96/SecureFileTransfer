#pragma once

#include "ui_ClientMainWindow.h"

#include "StateController.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(QWidget *parent = nullptr);

    void startStateController();

private:
    Ui::ClientMainWindowClass ui;
    StateController stateController;
};
