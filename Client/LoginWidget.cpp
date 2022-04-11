#include "stdafx.h"

#include "LoginWidget.h"
#include "NetworkController.h"

LoginWidget::LoginWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_login, &QPushButton::clicked, this, &LoginWidget::onPushButtonLoginClicked);
}

void LoginWidget::loginSucceeded()
{
	QMessageBox::information(this, u8"消息", u8"登录成功", QMessageBox::Ok);
}

void LoginWidget::onPushButtonLoginClicked()
{
	auto requset = NetworkController::instance->newRequest<LoginRequest>();
	requset->username = ui.lineEdit_username_2->text();
	requset->password = QCryptographicHash::hash(ui.lineEdit_password->text().toUtf8(), QCryptographicHash::Algorithm::Sha256);
	emit sendRequest(requset, 0);
}
