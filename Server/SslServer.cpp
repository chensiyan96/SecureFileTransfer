#include "stdafx.h"

#include "SslServer.h"

SslServer::SslServer(QObject* parent)
    : QTcpServer(parent)
{
    // ����֤�飬�뿴�˲���
    // https://www.jianshu.com/p/a9497de4cbff

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
        //connect(socket, &QSslSocket::encrypted, this, &SslServer::ready);
        socket->startServerEncryption();      
    }
    else {
        delete socket;
    }
}
