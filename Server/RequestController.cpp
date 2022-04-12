#include "stdafx.h"

#include "RequestController.h"

RequestController* RequestController::instance = nullptr;

RequestController::RequestController(QObject *parent)
	: QObject(parent)
{
	QThreadPool::globalInstance()->setExpiryTimeout(10000);
}

QSharedPointer<Response> RequestController::handleRequest(QSharedPointer<Request> request)
{
	switch (request->type)
	{
	case AppLayerMessage::Type::REGISTER:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<RegisterRequest*>(request.get())));
	case AppLayerMessage::Type::LOGIN:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<LoginRequest*>(request.get())));
	case AppLayerMessage::Type::LOGOUT:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<LogoutRequest*>(request.get())));
	case AppLayerMessage::Type::DELETE_USER:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<DeleteUserRequest*>(request.get())));
	case AppLayerMessage::Type::LIST_FILES:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<ListFilesRequest*>(request.get())));
	case AppLayerMessage::Type::MAKE_DIRECTORY:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<MakeDirectoryRequest*>(request.get())));
	case AppLayerMessage::Type::MOVE_FILE:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<MoveFileRequest*>(request.get())));
	case AppLayerMessage::Type::COPY_FILE:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<CopyFileRequest*>(request.get())));
	case AppLayerMessage::Type::UPLOAD_FILE:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<UploadFileRequest*>(request.get())));
	case AppLayerMessage::Type::DOWNLOAD_FILE:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<DownloadFileRequest*>(request.get())));
	case AppLayerMessage::Type::REMOVE_FILE:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<RemoveFileRequest*>(request.get())));
	case AppLayerMessage::Type::UPLOAD_DATA:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<UploadDataRequest*>(request.get())));
	case AppLayerMessage::Type::DOWNLOAD_DATA:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<DownloadDataRequest*>(request.get())));
	case AppLayerMessage::Type::CANCEL_TRANSFER:
		return QSharedPointer<Response>(handleRequest(dynamic_cast<CancelTransferRequest*>(request.get())));
	default:
		return nullptr;
	}
}

void RequestController::handleRequestAsync(QSharedPointer<Request> request)
{
	QtConcurrent::run(QThreadPool::globalInstance(), this, &RequestController::handleRequestConcurrent, request);
}

RegisterResponse* RequestController::handleRequest(const RegisterRequest* request)
{
	return new RegisterResponse(request->id);
}

LoginResponse* RequestController::handleRequest(const LoginRequest* request)
{
	return new LoginResponse(request->id);
}

LogoutResponse* RequestController::handleRequest(const LogoutRequest* request)
{
	return new LogoutResponse(request->id);
}

DeleteUserResponse* RequestController::handleRequest(const DeleteUserRequest* request)
{
	return new DeleteUserResponse(request->id);
}

ListFilesResponse* RequestController::handleRequest(const ListFilesRequest* request)
{
	return new ListFilesResponse(request->id);
}

MakeDirectoryResponse* RequestController::handleRequest(const MakeDirectoryRequest* request)
{
	return new MakeDirectoryResponse(request->id);
}

MoveFileResponse* RequestController::handleRequest(const MoveFileRequest* request)
{
	return new MoveFileResponse(request->id);
}

CopyFileResponse* RequestController::handleRequest(const CopyFileRequest* request)
{
	return new CopyFileResponse(request->id);
}

UploadFileResponse* RequestController::handleRequest(const UploadFileRequest* request)
{
	return new UploadFileResponse(request->id);
}

DownloadFileResponse* RequestController::handleRequest(const DownloadFileRequest* request)
{
	return new DownloadFileResponse(request->id);
}

RemoveFileResponse* RequestController::handleRequest(const RemoveFileRequest* request)
{
	return new RemoveFileResponse(request->id);
}

UploadDataResponse* RequestController::handleRequest(const UploadDataRequest* request)
{
	return new UploadDataResponse(request->id);
}

DownloadDataResponse* RequestController::handleRequest(const DownloadDataRequest* request)
{
	return new DownloadDataResponse(request->id);
}

CancelTransferResponse* RequestController::handleRequest(const CancelTransferRequest* request)
{
	return new CancelTransferResponse(request->id);
}

void RequestController::handleRequestConcurrent(QSharedPointer<Request> request)
{
	emit sendResponse(handleRequest(request));	
}
