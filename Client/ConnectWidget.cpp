#include "stdafx.h"

#include "ConnectWidget.h"

ConnectWidget::ConnectWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.label_connectionState->setText(u8"等待连接");
	connect(ui.pushButton_connect, &QPushButton::clicked, this, &ConnectWidget::onPushButtonConnectClicked);
}

void ConnectWidget::connectionSucceeded()
{
	ui.label_connectionState->setText(u8"连接成功");
	QMessageBox::information(this, u8"消息", u8"建立加密连接成功", QMessageBox::Ok);
}

void ConnectWidget::onPushButtonConnectClicked()
{
	auto host = ui.lineEdit_host->text();
	auto port = ui.lineEdit_port->text().toUShort();
	ui.pushButton_connect->setDisabled(true);
	ui.pushButton_connect->update();
	ui.label_connectionState->setText(u8"正在连接");
	emit connectToHost(host, port);
}
