#include "stdafx.h"

#include "ServerWidget.h"

#include "../SecureFileTransfer/SecureFileTransfer.h"
#include "../DatabaseAccess/DatabaseAccess.h"

ServerWidget::ServerWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ServerWidget::~ServerWidget()
{
}
