#pragma once

class SslServer : public QTcpServer
{
	Q_OBJECT

public:
	explicit SslServer(QObject *parent = nullptr);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	void ready();
	void errors(const QList<QSslError>& err);

private:
	QSslCertificate certificate;
	QSslKey privateKey;
};
