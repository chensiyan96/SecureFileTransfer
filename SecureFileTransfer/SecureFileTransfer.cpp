#include "stdafx.h"

#include "SecureFileTransfer.h"

void SecureFileTransfer::testServer()
{
	auto server = new QTcpServer;
	QObject::connect(server, &QTcpServer::newConnection, this, &SecureFileTransfer::newConnectionSlot);
	server->listen(QHostAddress::LocalHost, 8888);
}

void SecureFileTransfer::testClient()
{
	QThread::sleep(1);
	auto client = new QTcpSocket;
	client->connectToHost(QHostAddress::LocalHost, 8888);
}

void SecureFileTransfer::newConnectionSlot()
{
	QMessageBox::information(nullptr, u8"��Ϣ", u8"���ӳɹ�", QMessageBox::Ok);
}
