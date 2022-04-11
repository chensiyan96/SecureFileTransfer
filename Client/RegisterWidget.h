#pragma once

#include "ui_RegisterWidget.h"

#include "../SecureFileTransfer/AppLayerMessage.h"

class RegisterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RegisterWidget(QWidget *parent = nullptr);

	void registerSucceeded();

signals:
	void sendRequest(QSharedPointer<Request> request, int priority);

private:
	void onPushButtonRegisterClicked();

private:
	Ui::RegisterWidget ui;
};
