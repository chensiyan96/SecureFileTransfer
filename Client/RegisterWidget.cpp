#include "stdafx.h"

#include "RegisterWidget.h"
#include "NetworkController.h"

RegisterWidget::RegisterWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_register, &QPushButton::clicked, this, &RegisterWidget::onPushButtonRegisterClicked);
}

void RegisterWidget::succeeded()
{
	QMessageBox::information(this, u8"消息", u8"注册成功", QMessageBox::Ok);
}

void RegisterWidget::failed(QString message)
{
	QMessageBox::critical(this, u8"错误", message, QMessageBox::Ok);
	ui.pushButton_register->setDisabled(false);
}

void RegisterWidget::onPushButtonRegisterClicked()
{
	auto password1 = ui.lineEdit_password1->text();
	auto password2 = ui.lineEdit_password2->text();
	if (password1 != password2)
	{
		QMessageBox::critical(this, u8"错误", u8"两次输入的密码不一致", QMessageBox::Ok);
		return;
	}
	ui.pushButton_register->setDisabled(true);
	auto request = NetworkController::instance->newRequest<RegisterRequest>();;
	request->username = ui.lineEdit_username->text();
	request->password = QCryptographicHash::hash(password1.toUtf8(), QCryptographicHash::Algorithm::Sha256);
	NetworkController::instance->sendRequest(request, 0);
}
