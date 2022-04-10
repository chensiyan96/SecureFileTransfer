#pragma once

#include "ui_RegisterWidget.h"

class RegisterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RegisterWidget(QWidget *parent = nullptr);
	~RegisterWidget();

private:
	Ui::RegisterWidget ui;
};
