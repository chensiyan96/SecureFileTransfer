#pragma once

#include "ui_ServerWidget.h"

class ServerWidget : public QWidget
{
	Q_OBJECT

public:
	static ServerWidget* instance;

public:
	explicit ServerWidget(QWidget* parent = nullptr);

signals:
	void setCertificate(QString path);
	void setPrivateKey(QString path);
	void setDatabase(QString path);
	void setLogFile(QString path);

private:
	void selectFile(QLineEdit* lineEdit);

private:
	Ui::ServerWidget ui;
};
