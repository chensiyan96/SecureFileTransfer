#include "stdafx.h"

#include "StateController.h"
#include "NetworkController.h"
#include "RequestController.h"
#include "ServerWidget.h"

#include"../DatabaseAccess/DatabaseAccess.h"

StateController* StateController::instance = nullptr;

StateController::StateController(QObject* parent)
	: QObject(parent)
{
    // ����֤�飬�뿴�˲���
    // https://www.jianshu.com/p/a9497de4cbff

    QFile certFile("D:/x509/server.crt");
    certFile.open(QIODevice::ReadOnly);
    certificate = QSslCertificate(&certFile, QSsl::Pem);

    QFile keyFile("D:/x509/server.key");
    keyFile.open(QIODevice::ReadOnly);
    privateKey = QSslKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "root");

    setupState = new SetupState(&stateMachine);
    mainState = new MainState(&stateMachine);

    setupState->addTransition(setupState, &SetupState::setupFinished, mainState);
    mainState->addTransition(mainState, &MainState::stopFinished, mainState);

    stateMachine.setInitialState(setupState);
    stateMachine.start();
}

void SetupState::setCertificate(QString path)
{
    QFile certFile(path);
    certFile.open(QIODevice::ReadOnly);
    StateController::instance->certificate = QSslCertificate(&certFile, QSsl::Pem);
    if (StateController::instance->certificate.isNull())
    {
        QMessageBox::critical(nullptr, u8"����", u8"֤����Ч", QMessageBox::Ok);
    }
}

void SetupState::setPrivateKey(QString path)
{
    QFile keyFile(path);
    keyFile.open(QIODevice::ReadOnly);
    bool ok;
    QString password = QInputDialog::getText(nullptr, u8"��Կ����", u8"����������", QLineEdit::Password, 0, &ok);
    if (ok)
    {
        StateController::instance->privateKey = QSslKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, password.toUtf8());
    }
    if (StateController::instance->privateKey.isNull())
    {
        QMessageBox::critical(nullptr, u8"����", u8"��Կ��Ч", QMessageBox::Ok);
    }
}

void SetupState::setDatabase(QString path)
{
    databaseInit = DatabaseAccess::instance.init(path);
    if (!databaseInit)
    {
        QMessageBox::critical(nullptr, u8"����", u8"�������ݿ�ʧ��", QMessageBox::Ok);
    }
}

void SetupState::setup()
{
    if (StateController::instance->certificate.isNull())
    {
        QMessageBox::critical(nullptr, u8"����", u8"δ������ȷ��֤��", QMessageBox::Ok);
        return;
    }
    if (StateController::instance->privateKey.isNull())
    {
        QMessageBox::critical(nullptr, u8"����", u8"δ������ȷ����Կ", QMessageBox::Ok);
        return;
    }
    if (!databaseInit)
    {
        QMessageBox::critical(nullptr, u8"����", u8"δ�������ݿ�", QMessageBox::Ok);
        return;
    }
    emit setupFinished();
}

void SetupState::onEntry(QEvent* event)
{
    connect(ServerWidget::instance, &ServerWidget::setCertificate, this, &SetupState::setCertificate);
    connect(ServerWidget::instance, &ServerWidget::setPrivateKey, this, &SetupState::setPrivateKey);
    connect(ServerWidget::instance, &ServerWidget::setDatabase, this, &SetupState::setDatabase);
    connect(ServerWidget::instance, &ServerWidget::startServer, this, &SetupState::setup);
    ServerWidget::instance->onSetupStateEntry();
}

void SetupState::onExit(QEvent* event)
{
    ServerWidget::instance->onSetupStateExit();
    ServerWidget::instance->disconnect(this);
}

MainState::~MainState()
{
    if (serverThread.isRunning())
    {
        serverThread.quit();
        serverThread.wait();
    }
}

void MainState::onEntry(QEvent* event)
{
    RequestController::instance = new RequestController;
    auto server = new SslServer;
    this->server.reset(server);
    server->listen(QHostAddress::LocalHost, ServerWidget::instance->getPort());
    server->moveToThread(&serverThread);
    serverThread.start();
    ServerWidget::instance->onMainStateEntry();
}

void MainState::onExit(QEvent* event)
{
    ServerWidget::instance->onMainStateExit();
    serverThread.quit();
    serverThread.wait();
    server.reset(nullptr);
    delete RequestController::instance;
    RequestController::instance = nullptr;
}
