#include "stdafx.h"

#include "ServerWidget.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"
#include "../DatabaseAccess/DatabaseAccess.h"

ServerWidget* ServerWidget::instance = nullptr;

ServerWidget::ServerWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton_certPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_certPath);
		});
	connect(ui.pushButton_keyPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_keyPath);
		});
	connect(ui.pushButton_databasePath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_databasePath);
		});
	connect(ui.pushButton_logPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_logPath);
		});

	connect(ui.lineEdit_certPath, &QLineEdit::textChanged,
		[this]() {
			emit setCertificate(ui.lineEdit_certPath->text());
		});
	connect(ui.lineEdit_keyPath, &QLineEdit::textChanged,
		[this]() {
			emit setPrivateKey(ui.lineEdit_keyPath->text());
		});
	connect(ui.lineEdit_databasePath, &QLineEdit::textChanged,
		[this]() {
			emit setDatabase(ui.lineEdit_databasePath->text());
		});
	connect(ui.lineEdit_logPath, &QLineEdit::textChanged,
		[this]() {
			setLogFile(ui.lineEdit_logPath->text());
		});

	connect(ui.pushButton_startServer, &QPushButton::clicked, this, &ServerWidget::startServer);
	connect(ui.pushButton_stopServer, &QPushButton::clicked, this, &ServerWidget::stopServer);
	ui.pushButton_stopServer->setDisabled(true);
}

quint16 ServerWidget::getPort() const
{
	return quint16(ui.spinBox_port->value());
}

void ServerWidget::onSetupStateEntry()
{
	ui.pushButton_certPath->setDisabled(false);
	ui.pushButton_keyPath->setDisabled(false);
	ui.pushButton_databasePath->setDisabled(false);
	ui.pushButton_logPath->setDisabled(false);
	ui.pushButton_addAccess->setDisabled(false);
	ui.pushButton_deleteAccess->setDisabled(false);
	ui.pushButton_clearAccess->setDisabled(false);
	ui.pushButton_startServer->setDisabled(false);
}

void ServerWidget::onSetupStateExit()
{
	ui.pushButton_certPath->setDisabled(true);
	ui.pushButton_keyPath->setDisabled(true);
	ui.pushButton_databasePath->setDisabled(true);
	ui.pushButton_logPath->setDisabled(true);
	ui.pushButton_addAccess->setDisabled(true);
	ui.pushButton_deleteAccess->setDisabled(true);
	ui.pushButton_clearAccess->setDisabled(true);
	ui.pushButton_startServer->setDisabled(true);
}

void ServerWidget::onMainStateEntry()
{
	ui.pushButton_stopServer->setDisabled(false);
}

void ServerWidget::onMainStateExit()
{
	ui.pushButton_stopServer->setDisabled(true);
}

void ServerWidget::outputLog(const QString& log)
{
	while (ui.listWidget_log->takeItem(100) != nullptr)
	{
		ui.listWidget_log->removeItemWidget(ui.listWidget_log->takeItem(0));
	}	
	ui.listWidget_log->addItem(log);
	if (logFile.isOpen())
	{
		logFile.write(log.toUtf8());
		logFile.write("\n");
		logFile.flush();
	}
}

void ServerWidget::selectFile(QLineEdit* lineEdit)
{
	auto s = QFileDialog::getOpenFileName(this, u8"Ñ¡ÔñÎÄ¼þ");
	if (!s.isEmpty())
	{
		lineEdit->setText(s);
	}
}

void ServerWidget::setLogFile(QString path)
{
	if (logFile.isOpen())
	{
		logFile.close();
	}
	logFile.setFileName(path);
	logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}
