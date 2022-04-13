#include "stdafx.h"

#include "ClientMainWindow.h"
#include "NetworkController.h"

ClientMainWindow* ClientMainWindow::instance = nullptr;

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.action_quit, &QAction::triggered, this, &QWidget::close);
    connect(ui.action_register, &QAction::triggered, this, &ClientMainWindow::registerTriggered);
    connect(ui.action_login, &QAction::triggered, this, &ClientMainWindow::loginTriggered);

    connect(ui.action_logout, &QAction::triggered,
        [this]() {
            auto request = QSharedPointer<Request>(NetworkController::instance->newRequest<LogoutRequest>());
            NetworkController::instance->sendRequest(request, 0);
        });
    connect(ui.action_deleteUser, &QAction::triggered,
        [this]() {
            if (QMessageBox::warning(this, u8"警告", u8"此操作不可逆，真的要删除账号吗？",
                QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
            {
                auto request = QSharedPointer<Request>(NetworkController::instance->newRequest<DeleteUserRequest>());
                NetworkController::instance->sendRequest(request, 0);
            }
        });
    
    connect(ui.action_connect, &QAction::triggered, this, &ClientMainWindow::connectToHostTriggered);
    connect(ui.action_disconnect, &QAction::triggered, this, &ClientMainWindow::disconnectFromHostTriggered);

    connect(ui.action_uploadFile, &QAction::triggered, this, &ClientMainWindow::onActionUploadFileTriggered);
    connect(ui.action_downloadFile, &QAction::triggered, this, &ClientMainWindow::onActionDownloadFileTriggered);

    ui.tableView_local->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_local->setModel(&localFileSystemModel);
    ui.tableView_remote->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_remote->setModel(&remoteFileSystemModel);
    ui.tableView_transfer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ClientMainWindow::onMainStateEntry()
{
    connect(NetworkController::instance, &NetworkController::receivedResponse, &remoteFileSystemModel, &RemoteFileSystemModel::checkResponse);
}

void ClientMainWindow::onMainStateExit()
{
    NetworkController::instance->disconnect(&remoteFileSystemModel);
}

void ClientMainWindow::onActionUploadFileTriggered()
{
    auto selectedRows = ui.tableView_local->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要上传的文件", QMessageBox::Ok);
        return;
    }
    for (const auto& item : selectedRows)
    {
        qDebug() << item.row();
        qDebug() << item.flags();
    }
}

void ClientMainWindow::onActionDownloadFileTriggered()
{
    auto selectedRows = ui.tableView_remote->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要下载的文件", QMessageBox::Ok);
        return;
    }
    for (const auto& item : selectedRows)
    {
        qDebug() << item.row();
    }
}

void ClientMainWindow::onActionDeleteFileTriggered()
{
    auto selectedRows = ui.tableView_remote->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要删除的文件", QMessageBox::Ok);
        return;
    }
    for (const auto& item : selectedRows)
    {
        qDebug() << item.row();
    }
}
