#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"

class NetworkController : public QObject
{
	Q_OBJECT

public:
	explicit NetworkController(QSslSocket* socket, QObject* parent = nullptr);

	void start(); // slot

private:
	void readRequest();
	void writeResponse();

	QSharedPointer<Response> handleRequest(QSharedPointer<Request> request);
	RegisterResponse* handleRequest(const RegisterRequest* request);
	LoginResponse* handleRequest(const LoginRequest* request);
	LogoutResponse* handleRequest(const LogoutRequest* request);
	DeleteUserResponse* handleRequest(const DeleteUserRequest* request);
	ListFilesResponse* handleRequest(const ListFilesRequest* request);
	MakeDirectoryResponse* handleRequest(const MakeDirectoryRequest* request);
	MoveFileResponse* handleRequest(const MoveFileRequest* request);
	CopyFileResponse* handleRequest(const CopyFileRequest* request);
	UploadFileResponse* handleRequest(const UploadFileRequest* request);
	DownloadFileResponse* handleRequest(const DownloadFileRequest* request);
	RemoveFileResponse* handleRequest(const RemoveFileRequest* request);
	UploadDataResponse* handleRequest(const UploadDataRequest* request);
	DownloadDataResponse* handleRequest(const DownloadDataRequest* request);
	CancelTransferResponse* handleRequest(const CancelTransferRequest* request);

private:
	QTimer timer;
	QScopedPointer<QSslSocket> socket;
	QByteArray receiveBuffer;
};
