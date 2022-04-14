#pragma once

#include "ui_ServerWidget.h"

class ServerWidget : public QWidget
{
	Q_OBJECT

public:
	static ServerWidget* instance;

public:
	explicit ServerWidget(QWidget* parent = nullptr);

	quint16 getPort() const;
	QVector<QString> getAccessibleDirectories() const;

	void onSetupStateEntry();
	void onSetupStateExit();
	void onMainStateEntry();
	void onMainStateExit();

	void outputLog(const QString& log);

signals:
	void setCertificate(QString path);
	void setPrivateKey(QString path);
	void setDatabase(QString path);

	void startServer();
	void stopServer();

private:
	void selectFile(QLineEdit* lineEdit);
	void setLogFile(QString path);

private:
	Ui::ServerWidget ui;
	QFile logFile;
};
