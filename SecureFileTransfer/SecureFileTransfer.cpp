#include "stdafx.h"

#include "SecureFileTransfer.h"
#include "SslServer.h"

void SecureFileTransfer::testServer()
{
	// 如果不支持openssl，请看此博客
	// https://blog.csdn.net/YinShiJiaW/article/details/106220932
	qDebug() << u8"OpenSSL支持情况:" << QSslSocket::supportsSsl();
	qDebug() << u8"OpenSSL版本:" << QSslSocket::sslLibraryBuildVersionString();

	// 配置证书，请看此博客
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
	QMessageBox::information(nullptr, u8"消息", u8"连接成功", QMessageBox::Ok);
}
