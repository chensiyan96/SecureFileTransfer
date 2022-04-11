#include "stdafx.h"

#include "SslServer.h"
#include "AppLayerMessage.h"

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
    auto socket = new QSslSocket(this);
    socket->setLocalCertificate(certificate);
    socket->setPrivateKey(privateKey);
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(socket);
        connect(socket, &QSslSocket::encrypted, this, &SslServer::ready);
        socket->startServerEncryption();
        socket->waitForReadyRead(); 
        Request::deserialize(socket->readAll());
        socket->write(LoginResponse(1).serialize());
    }
    else {
        delete socket;
    }
}

void SslServer::ready()
{
    //QMessageBox::information(nullptr, u8"消息", u8"加密完成", QMessageBox::Ok);
}

void SslServer::errors(const QList<QSslError>& err)
{
    //QMessageBox::critical(nullptr, u8"错误", u8"加密失败", QMessageBox::Ok);
}
