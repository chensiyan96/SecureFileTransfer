#include "stdafx.h"

#include "SecureFileTransfer.h"
#include "SslServer.h"

void SecureFileTransfer::testServer()
{
	// �����֧��openssl���뿴�˲���
	// https://blog.csdn.net/YinShiJiaW/article/details/106220932
	qDebug() << u8"OpenSSL֧�����:" << QSslSocket::supportsSsl();
	qDebug() << u8"OpenSSL�汾:" << QSslSocket::sslLibraryBuildVersionString();

	// ����֤�飬�뿴�˲���
	// https://www.jianshu.com/p/a9497de4cbff

	auto server = new SslServer;
	QObject::connect(server, &QTcpServer::newConnection, this, &SecureFileTransfer::newConnectionSlot);
	server->listen(QHostAddress::LocalHost, 8888);
}

void SecureFileTransfer::testClient()
{
	QThread::sleep(1);
	auto client = new QSslSocket;
	client->setPeerVerifyMode(QSslSocket::QueryPeer);
	client->connectToHostEncrypted("localhost", 8888);
}

void SecureFileTransfer::newConnectionSlot()
{
	QMessageBox::information(nullptr, u8"��Ϣ", u8"���ӳɹ�", QMessageBox::Ok);
}
