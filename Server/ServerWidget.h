#pragma once

#include "ui_ServerWidget.h"

class ServerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ServerWidget(QWidget* parent = nullptr);
	~ServerWidget();

private:
	Ui::ServerWidget ui;
};
