#include "stdafx.h"

#include "SslServer.h"

SslServer::SslServer(QObject *parent)
	: QTcpServer(parent)
{
    QFile certFile("D:/x509/server.crt");
    certFile.open(QIODevice::ReadOnly);
    certificate = QSslCertificate(&certFile, QSsl::Pem);
    qDebug() << certificate.isNull();
    qDebug() << certificate.isSelfSigned();

    QFile keyFile("D:/x509/server.key");
    keyFile.open(QIODevice::ReadOnly);
    privateKey = QSslKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "root");
    qDebug() << privateKey.isNull();
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    auto serverSocket = new QSslSocket(this);
    serverSocket->setLocalCertificate(certificate);
    serverSocket->setPrivateKey(privateKey);
    serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(serverSocket);
        connect(serverSocket, &QSslSocket::encrypted, this, &SslServer::ready);
        serverSocket->startServerEncryption();
    }
    else {
        delete serverSocket;
    }
}

void SslServer::ready()
{
    QMessageBox::information(nullptr, u8"消息", u8"加密完成", QMessageBox::Ok);
}

void SslServer::errors(const QList<QSslError>& err)
{
    QMessageBox::critical(nullptr, u8"错误", u8"加密失败", QMessageBox::Ok);
}
