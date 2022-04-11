#pragma once

#include "NetworkController.h"

#include <memory>
#include <unordered_set>

class SslServer : public QTcpServer
{
	Q_OBJECT

public:
	explicit SslServer(QObject* parent = nullptr);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	std::unordered_set<std::unique_ptr<NetworkController>> allNetworkControllers;
	QSslCertificate certificate;
	QSslKey privateKey;
};
