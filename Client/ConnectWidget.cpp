#include "stdafx.h"

#include "ConnectWidget.h"

ConnectWidget::ConnectWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.label_connectionState->setText(u8"等待连接");
	connect(ui.pushButton_connect, &QPushButton::clicked, this, &ConnectWidget::onPushButtonConnectClicked);
}

void ConnectWidget::succeeded()
{
	ui.label_connectionState->setText(u8"连接成功");
	QMessageBox::information(this, u8"消息", u8"建立加密连接成功", QMessageBox::Ok);
}

void ConnectWidget::failed(QString message)
{
	QMessageBox::critical(this, u8"错误", message, QMessageBox::Ok);
	ui.pushButton_connect->setDisabled(false);
}

void ConnectWidget::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
	emit canceled();
}

void ConnectWidget::onPushButtonConnectClicked()
{
	auto host = ui.lineEdit_host->text();
	quint16 port = ui.spinBox_port->value();
	ui.pushButton_connect->setDisabled(true);
	ui.label_connectionState->setText(u8"正在连接");
	emit connectToHost(host, port);
}
