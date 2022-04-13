#pragma once

#include "ui_ConnectWidget.h"

class ConnectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ConnectWidget(QWidget *parent = nullptr);

	void succeeded();
	void failed(QString message);

signals:
	void canceled();
	void connectToHost(QString host, quint16 port);

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void onPushButtonConnectClicked();

private:
	Ui::ConnectWidget ui;
};
