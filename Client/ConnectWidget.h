#pragma once

#include "ui_ConnectWidget.h"

class ConnectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ConnectWidget(QWidget *parent = nullptr);

signals:
	void connectToHost(QString host, quint16 port);

public:
	void encryptionSucceeded();

private:
	void onPushButtonConnectClicked();

private:
	Ui::ConnectWidget ui;
};
