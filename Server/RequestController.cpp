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
	if (request->username.size() > 255 || request->username.size() == 0)
	{
		auto response = new RegisterResponse(request->id);
		response->result = RegisterResponse::Result::INVALID_ARGUMENT;
		return response;
	}
	return userService.handleRequest(request);
}

LoginResponse* RequestController::handleRequest(const LoginRequest* request)
{
	if (request->username.size() > 255 || request->username.size() == 0)
	{
		auto response = new LoginResponse(request->id);
		response->result = LoginResponse::Result::INVALID_ARGUMENT;
		return response;
	}
	return userService.handleRequest(request);
}

LogoutResponse* RequestController::handleRequest(const LogoutRequest* request)
{
	auto response = new LogoutResponse(request->id);
	return response;
}

DeleteUserResponse* RequestController::handleRequest(const DeleteUserRequest* request)
{
	auto response = new DeleteUserResponse(request->id);
	return response;
}

ListFilesResponse* RequestController::handleRequest(const ListFilesRequest* request)
{
	auto response = new ListFilesResponse(request->id);
	return response;
}

MakeDirectoryResponse* RequestController::handleRequest(const MakeDirectoryRequest* request)
{
	auto response = new MakeDirectoryResponse(request->id);
	return response;
}

MoveFileResponse* RequestController::handleRequest(const MoveFileRequest* request)
{
	auto response = new MoveFileResponse(request->id);
	return response;
}

CopyFileResponse* RequestController::handleRequest(const CopyFileRequest* request)
{
	auto response = new CopyFileResponse(request->id);
	return response;
}

UploadFileResponse* RequestController::handleRequest(const UploadFileRequest* request)
{
	auto response = new UploadFileResponse(request->id);
	return response;
}

DownloadFileResponse* RequestController::handleRequest(const DownloadFileRequest* request)
{
	auto response = new DownloadFileResponse(request->id);
	return response;
}

RemoveFileResponse* RequestController::handleRequest(const RemoveFileRequest* request)
{
	auto response = new RemoveFileResponse(request->id);
	return response;
}

UploadDataResponse* RequestController::handleRequest(const UploadDataRequest* request)
{
	auto response = new UploadDataResponse(request->id);
	return response;
}

DownloadDataResponse* RequestController::handleRequest(const DownloadDataRequest* request)
{
	auto response = new DownloadDataResponse(request->id);
	return response;
}

CancelTransferResponse* RequestController::handleRequest(const CancelTransferRequest* request)
{
	auto response = new CancelTransferResponse(request->id);
	return response;
}

void RequestController::handleRequestConcurrent(QSharedPointer<Request> request)
{
	emit sendResponse(handleRequest(request));	
}
