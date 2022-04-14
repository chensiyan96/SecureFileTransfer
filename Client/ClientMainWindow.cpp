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
    connect(ui.action_deleteFile, &QAction::triggered, this, &ClientMainWindow::onActionDeleteFileTriggered);

    ui.tableView_local->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_local->setModel(&localFileSystemModel);
    ui.tableView_remote->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_remote->setModel(&remoteFileSystemModel);
    ui.tableView_transfer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_transfer->setModel(&transferModel);
}

void ClientMainWindow::onMainStateEntry()
{
    connect(NetworkController::instance, &NetworkController::receivedResponse, &remoteFileSystemModel, &RemoteFileSystemModel::checkResponse);
    connect(&transferModel, &TransferModel::taskFinished, &localFileSystemModel, &LocalFileSystemModel::refresh);
    connect(&transferModel, &TransferModel::taskFinished, &remoteFileSystemModel, &RemoteFileSystemModel::refresh);
    transferModel.onMainStateEntry();
    remoteFileSystemModel.refresh();
}

void ClientMainWindow::onMainStateExit()
{
    NetworkController::instance->disconnect(&remoteFileSystemModel);
    transferModel.onMainStateExit();
}

static QVector<QString> getFileNameVec(const QTableView& view, const FileSystemModel& model)
{
    auto selectedRows = view.selectionModel()->selectedRows();
    QVector<QString> result;
    result.reserve(selectedRows.size());
    for (const auto& item : selectedRows)
    {
        auto fileName = model.getFileName(item.row());
        qDebug() << fileName;
        if (!fileName.isEmpty())
        {
            result.push_back(fileName);
        }
    }
    return result;
}

void ClientMainWindow::onActionUploadFileTriggered()
{
    if (!ui.tableView_local->hasFocus())
    {
        QMessageBox::information(this, u8"消息", u8"请将鼠标焦点设置在本地目录窗口中", QMessageBox::Ok);
        return;
    }
    auto fileNameVec = getFileNameVec(*ui.tableView_local, localFileSystemModel);
    if (fileNameVec.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要上传的文件", QMessageBox::Ok);
        return;
    }
    for (auto fileName : fileNameVec)
    {
        transferModel.addTask(TransferTask::Type::UPLOAD,
            remoteFileSystemModel.getPath(fileName), localFileSystemModel.getPath(fileName));
    }
}

void ClientMainWindow::onActionDownloadFileTriggered()
{
    if (!ui.tableView_remote->hasFocus())
    {
        QMessageBox::information(this, u8"消息", u8"请将鼠标焦点设置在远程目录窗口中", QMessageBox::Ok);
        return;
    }
    auto fileNameVec = getFileNameVec(*ui.tableView_remote, remoteFileSystemModel);
    if (fileNameVec.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要下载的文件", QMessageBox::Ok);
        return;
    }
    for (auto fileName : fileNameVec)
    {
        transferModel.addTask(TransferTask::Type::DOWNLOAD,
            localFileSystemModel.getPath(fileName), remoteFileSystemModel.getPath(fileName));
    }
}

void ClientMainWindow::onActionDeleteFileTriggered()
{
    QTableView* view = nullptr;
    FileSystemModel* model = nullptr;
    if (ui.tableView_local->hasFocus())
    {
        view = ui.tableView_local;
        model = &localFileSystemModel;
    }
    else if (ui.tableView_remote->hasFocus())
    {
        view = ui.tableView_remote;
        model = &remoteFileSystemModel;
    }
    else
    {
        QMessageBox::information(this, u8"消息", u8"请将鼠标焦点设置在本地目录窗口或远程目录窗口中", QMessageBox::Ok);
        return;
    }
    auto fileNameVec = getFileNameVec(*view, *model);
    if (fileNameVec.isEmpty())
    {
        QMessageBox::information(this, u8"消息", u8"未选择要删除的文件", QMessageBox::Ok);
        return;
    }
    if (model == &localFileSystemModel)
    {
        for (auto fileName : fileNameVec)
        {
            auto path = localFileSystemModel.getPath(fileName);
            localFileSystemModel.fileService.removeFile(path);
        }
        localFileSystemModel.refresh();
    }
    else
    {
        for (auto fileName : fileNameVec)
        {
            auto request = NetworkController::instance->newRequest<RemoveFileRequest>();
            request->path = remoteFileSystemModel.getPath(fileName);
            NetworkController::instance->sendRequest(request, 1);
        }
        remoteFileSystemModel.refresh();
    }
}
