#include "stdafx.h"

#include "SslServer.h"

SslServer::SslServer(QObject* parent)
    : QTcpServer(parent)
{
    // 配置证书，请看此博客
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
    auto socket = new QSslSocket;
    socket->setLocalCertificate(certificate);
    socket->setPrivateKey(privateKey);
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        auto newNetworkController = new NetworkController(socket);
        connect(this, &SslServer::newConnection, newNetworkController, &NetworkController::start);
        addPendingConnection(socket);
        allNetworkControllers.emplace(newNetworkController);
    }
    else {
        delete socket;
    }
}
