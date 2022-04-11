#pragma once

#include "ui_LoginWidget.h"

#include "../SecureFileTransfer/AppLayerMessage.h"

class LoginWidget : public QWidget
{
	Q_OBJECT

public:
	explicit LoginWidget(QWidget* parent = nullptr);

	void loginSucceeded();

signals:
	void sendRequest(QSharedPointer<Request> request, int priority);

private:
	void onPushButtonLoginClicked();

private:
	Ui::LoginWidget ui;
};
