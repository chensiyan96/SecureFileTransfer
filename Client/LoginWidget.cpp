#include "stdafx.h"

#include "LoginWidget.h"
#include "NetworkController.h"

LoginWidget::LoginWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_login, &QPushButton::clicked, this, &LoginWidget::onPushButtonLoginClicked);
}

void LoginWidget::succeeded()
{
	QMessageBox::information(this, u8"消息", u8"登录成功", QMessageBox::Ok);
}

void LoginWidget::failed(QString message)
{
	QMessageBox::critical(this, u8"错误", message, QMessageBox::Ok);
	ui.pushButton_login->setDisabled(false);
}

void LoginWidget::onPushButtonLoginClicked()
{
	ui.pushButton_login->setDisabled(true);
	auto request = NetworkController::instance->newRequest<LoginRequest>();
	request->username = ui.lineEdit_username_2->text();
	request->password = QCryptographicHash::hash(ui.lineEdit_password->text().toUtf8(), QCryptographicHash::Algorithm::Sha256);
	NetworkController::instance->sendRequest(request, 0);
}
