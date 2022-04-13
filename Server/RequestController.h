#pragma once

#include "UserService.h"
#include "../SecureFileTransfer/FileService.h"

class RequestController : public QObject
{
	Q_OBJECT

public:
	static RequestController* instance;

public:
	explicit RequestController(QObject* parent = nullptr);

public:
	QSharedPointer<Response> handleRequest(QSslSocket* socket, QSharedPointer<Request> request);
	void handleRequestAsync(QSslSocket* socket, QSharedPointer<Request> request);

signals:
	void sendResponse(QSharedPointer<Response> response);

private:
	RegisterResponse* handleRequest(QSslSocket* socket, const RegisterRequest* request);
	LoginResponse* handleRequest(QSslSocket* socket, const LoginRequest* request);
	LogoutResponse* handleRequest(QSslSocket* socket, const LogoutRequest* request);
	DeleteUserResponse* handleRequest(QSslSocket* socket, const DeleteUserRequest* request);
	ListFilesResponse* handleRequest(QSslSocket* socket, const ListFilesRequest* request);
	MakeDirectoryResponse* handleRequest(QSslSocket* socket, const MakeDirectoryRequest* request);
	MoveFileResponse* handleRequest(QSslSocket* socket, const MoveFileRequest* request);
	CopyFileResponse* handleRequest(QSslSocket* socket, const CopyFileRequest* request);
	UploadFileResponse* handleRequest(QSslSocket* socket, const UploadFileRequest* request);
	DownloadFileResponse* handleRequest(QSslSocket* socket, const DownloadFileRequest* request);
	RemoveFileResponse* handleRequest(QSslSocket* socket, const RemoveFileRequest* request);
	UploadDataResponse* handleRequest(QSslSocket* socket, const UploadDataRequest* request);
	DownloadDataResponse* handleRequest(QSslSocket* socket, const DownloadDataRequest* request);
	CancelTransferResponse* handleRequest(QSslSocket* socket, const CancelTransferRequest* request);

	void handleRequestConcurrent(QSslSocket* socket, QSharedPointer<Request> request);

	bool checkPath(QString path);

private:
	UserService userService;
	FileService fileService;
	std::map<quint32, std::unique_ptr<QFile>> openedFiles;
	QMutex openedFilesMutex;
};
