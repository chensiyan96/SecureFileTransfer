#pragma once

#include "ui_LoginWidget.h"

#include "../SecureFileTransfer/AppLayerMessage.h"

class LoginWidget : public QWidget
{
	Q_OBJECT

public:
	explicit LoginWidget(QWidget* parent = nullptr);

	void succeeded();
	void failed(QString message);

private:
	void onPushButtonLoginClicked();

private:
	Ui::LoginWidget ui;
};
