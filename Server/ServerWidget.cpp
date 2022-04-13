#include "stdafx.h"

#include "ServerWidget.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"
#include "../DatabaseAccess/DatabaseAccess.h"

ServerWidget* ServerWidget::instance = nullptr;

ServerWidget::ServerWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton_certPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_certPath);
		});
	connect(ui.pushButton_keyPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_keyPath);
		});
	connect(ui.pushButton_databasePath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_databasePath);
		});
	connect(ui.pushButton_logPath, &QPushButton::clicked,
		[this]() {
			selectFile(ui.lineEdit_logPath);
		});

	connect(ui.lineEdit_certPath, &QLineEdit::textChanged,
		[this]() {
			emit setCertificate(ui.lineEdit_certPath->text());
		});
	connect(ui.lineEdit_keyPath, &QLineEdit::textChanged,
		[this]() {
			emit setPrivateKey(ui.lineEdit_keyPath->text());
		});
	connect(ui.lineEdit_databasePath, &QLineEdit::textChanged,
		[this]() {
			emit setDatabase(ui.lineEdit_databasePath->text());
		});
	connect(ui.lineEdit_logPath, &QLineEdit::textChanged,
		[this]() {
			emit setLogFile(ui.lineEdit_logPath->text());
		});
}

void ServerWidget::selectFile(QLineEdit* lineEdit)
{
	auto s = QFileDialog::getOpenFileName(this, u8"Ñ¡ÔñÎÄ¼þ");
	if (!s.isEmpty())
	{
		lineEdit->setText(s);
	}
}
