#include "stdafx.h"

#include "RegisterWidget.h"
#include "NetworkController.h"

RegisterWidget::RegisterWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_register, &QPushButton::clicked, this, &RegisterWidget::onPushButtonRegisterClicked);
}

void RegisterWidget::registerSucceeded()
{
	QMessageBox::information(this, u8"��Ϣ", u8"ע��ɹ�", QMessageBox::Ok);
}

void RegisterWidget::onPushButtonRegisterClicked()
{
	auto password1 = ui.lineEdit_password1->text();
	auto password2 = ui.lineEdit_password2->text();
	if (password1 != password2)
	{
		QMessageBox::critical(this, u8"����", u8"������������벻һ��", QMessageBox::Ok);
		return;
	}
	auto requset = NetworkController::instance->newRequest<RegisterRequest>();;
	requset->username = ui.lineEdit_username->text();
	requset->password = QCryptographicHash::hash(password1.toUtf8(), QCryptographicHash::Algorithm::Sha256);
	emit registerRequest(requset);
}
