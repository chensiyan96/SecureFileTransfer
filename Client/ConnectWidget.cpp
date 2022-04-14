#include "stdafx.h"

#include "ConnectWidget.h"

ConnectWidget::ConnectWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.label_connectionState->setText(u8"�ȴ�����");
	connect(ui.pushButton_connect, &QPushButton::clicked, this, &ConnectWidget::onPushButtonConnectClicked);
}

void ConnectWidget::succeeded()
{
	ui.label_connectionState->setText(u8"���ӳɹ�");
	QMessageBox::information(this, u8"��Ϣ", u8"�����������ӳɹ�", QMessageBox::Ok);
}

void ConnectWidget::failed(QString message)
{
	QMessageBox::critical(this, u8"����", message, QMessageBox::Ok);
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
	ui.label_connectionState->setText(u8"��������");
	emit connectToHost(host, port);
}
