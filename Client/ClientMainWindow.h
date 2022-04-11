#pragma once

#include "ui_ClientMainWindow.h"

#include "../SecureFileTransfer/AppLayerMessage.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static ClientMainWindow* instance;

public:
    explicit ClientMainWindow(QWidget *parent = nullptr);

signals:
    void sendRequest(QSharedPointer<Request> request, int priority);

    void registerTriggered();
    void loginTriggered();

private:
    Ui::ClientMainWindowClass ui;
    QFileSystemModel fileSystemModel;
};
