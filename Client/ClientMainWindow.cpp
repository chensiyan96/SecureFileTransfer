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
            emit sendRequest(request, 0);
        });
    connect(ui.action_deleteUser, &QAction::triggered,
        [this]() {
            auto request = QSharedPointer<Request>(NetworkController::instance->newRequest<DeleteUserRequest>());
            emit sendRequest(request, 0);
        });
    
    connect(ui.action_connect, &QAction::triggered, this, &ClientMainWindow::connectToHostTriggered);
    connect(ui.action_disconnect, &QAction::triggered, this, &ClientMainWindow::disconnectFromHostTriggered);

    fileSystemModel.setRootPath(QDir::currentPath());
    ui.tableView_local->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_local->setModel(&fileSystemModel);
    ui.tableView_remote->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView_remote->setModel(&fileSystemModel);
    ui.tableView_transfer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
