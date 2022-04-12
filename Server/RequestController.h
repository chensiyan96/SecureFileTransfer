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
	QSharedPointer<Response> handleRequest(QSharedPointer<Request> request);
	void handleRequestAsync(QSharedPointer<Request> request);

signals:
	void sendResponse(QSharedPointer<Response> response);

private:
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

	void handleRequestConcurrent(QSharedPointer<Request> request);

private:
	UserService userService;
	FileService fileService;
};
