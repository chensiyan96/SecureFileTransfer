#pragma once

#include "ui_ClientMainWindow.h"

#include "../SecureFileTransfer/AppLayerMessage.h"

#include "FileSystemModel.h"
#include "TransferModel.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static ClientMainWindow* instance;

public:
    explicit ClientMainWindow(QWidget *parent = nullptr);

    void onMainStateEntry();
    void onMainStateExit();

signals:
    void sendRequest(QSharedPointer<Request> request, int priority);

    void registerTriggered();
    void loginTriggered();

    void connectToHostTriggered();
    void disconnectFromHostTriggered();

private:
    void onActionUploadFileTriggered();
    void onActionDownloadFileTriggered();
    void onActionDeleteFileTriggered();

private:
    Ui::ClientMainWindowClass ui;
    LocalFileSystemModel localFileSystemModel;
    RemoteFileSystemModel remoteFileSystemModel;
    TransferModel transferModel;
};
