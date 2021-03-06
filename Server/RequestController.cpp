#include "stdafx.h"

#include "RequestController.h"
#include "ServerWidget.h"

RequestController* RequestController::instance = nullptr;

RequestController::RequestController(QObject *parent)
	: QObject(parent)
{
	QThreadPool::globalInstance()->setExpiryTimeout(10000);
	fileService.accessibleDirectories = ServerWidget::instance->getAccessibleDirectories();
}

QSharedPointer<Response> RequestController::handleRequest(QSslSocket* socket, QSharedPointer<Request> request)
{
	switch (request->type)
	{
	case AppLayerMessage::Type::REGISTER:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<RegisterRequest*>(request.get())));
	case AppLayerMessage::Type::LOGIN:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<LoginRequest*>(request.get())));
	case AppLayerMessage::Type::LOGOUT:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<LogoutRequest*>(request.get())));
	case AppLayerMessage::Type::DELETE_USER:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<DeleteUserRequest*>(request.get())));
	case AppLayerMessage::Type::LIST_FILES:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<ListFilesRequest*>(request.get())));
	case AppLayerMessage::Type::MAKE_DIRECTORY:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<MakeDirectoryRequest*>(request.get())));
	case AppLayerMessage::Type::MOVE_FILE:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<MoveFileRequest*>(request.get())));
	case AppLayerMessage::Type::COPY_FILE:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<CopyFileRequest*>(request.get())));
	case AppLayerMessage::Type::UPLOAD_FILE:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<UploadFileRequest*>(request.get())));
	case AppLayerMessage::Type::DOWNLOAD_FILE:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<DownloadFileRequest*>(request.get())));
	case AppLayerMessage::Type::REMOVE_FILE:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<RemoveFileRequest*>(request.get())));
	case AppLayerMessage::Type::UPLOAD_DATA:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<UploadDataRequest*>(request.get())));
	case AppLayerMessage::Type::DOWNLOAD_DATA:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<DownloadDataRequest*>(request.get())));
	case AppLayerMessage::Type::CANCEL_TRANSFER:
		return QSharedPointer<Response>(handleRequest(socket, dynamic_cast<CancelTransferRequest*>(request.get())));
	default:
		return nullptr;
	}
}

void RequestController::handleRequestAsync(QSslSocket* socket, QSharedPointer<Request> request)
{
	QtConcurrent::run(QThreadPool::globalInstance(), this, &RequestController::handleRequestConcurrent, socket, request);
}

RegisterResponse* RequestController::handleRequest(QSslSocket* socket, const RegisterRequest* request)
{
	if (request->username.size() > 255 || request->username.size() == 0)
	{
		auto response = new RegisterResponse(request->id);
		response->result = RegisterResponse::Result::INVALID_ARGUMENT;
		return response;
	}
	return userService.handleRequest(socket, request);
}

LoginResponse* RequestController::handleRequest(QSslSocket* socket, const LoginRequest* request)
{
	if (request->username.size() > 255 || request->username.size() == 0)
	{
		auto response = new LoginResponse(request->id);
		response->result = LoginResponse::Result::INVALID_ARGUMENT;
		return response;
	}
	return userService.handleRequest(socket, request);
}

LogoutResponse* RequestController::handleRequest(QSslSocket* socket, const LogoutRequest* request)
{
	return userService.handleRequest(socket, request);
}

DeleteUserResponse* RequestController::handleRequest(QSslSocket* socket, const DeleteUserRequest* request)
{
	return userService.handleRequest(socket, request);
}

ListFilesResponse* RequestController::handleRequest(QSslSocket* socket, const ListFilesRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new ListFilesResponse(request->id);
	if (request->directory.isEmpty())
	{
		// ????????????????????
		fileService.getAccessibleDirectories(response->infoVec);
		return response;
	}
	if (!checkPath(request->directory))
	{
		response->result = ListFilesResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto result = fileService.listFiles(request->directory, response->infoVec);
		switch (result)
		{
		case FileService::Result::NO_SUCH_FILE:
			response->result = ListFilesResponse::Result::NO_SUCH_FILE;
			break;
		case FileService::Result::CANNOT_ACCESS:
			response->result = ListFilesResponse::Result::CANNOT_ACCESS;
			break;
		}
	}
	return response;
}

MakeDirectoryResponse* RequestController::handleRequest(QSslSocket* socket, const MakeDirectoryRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new MakeDirectoryResponse(request->id);
	if (!checkPath(request->path))
	{
		response->result = MakeDirectoryResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto result = fileService.makeDirectory(request->path);
		switch (result)
		{
		case FileService::Result::CANNOT_ACCESS:
			response->result = MakeDirectoryResponse::Result::CANNOT_ACCESS;
			break;
		case FileService::Result::FILE_EXISTS:
			response->result = MakeDirectoryResponse::Result::FILE_EXISTS;
			break;
		}
	}
	return response;
}

MoveFileResponse* RequestController::handleRequest(QSslSocket* socket, const MoveFileRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new MoveFileResponse(request->id);
	if (!checkPath(request->dst) || !checkPath(request->src))
	{
		response->result = MoveFileResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto result = fileService.moveFile(request->dst, request->src, request->force);
		switch (result)
		{
		case FileService::Result::NO_SUCH_FILE:
			response->result = MoveFileResponse::Result::NO_SUCH_FILE;
			break;
		case FileService::Result::CANNOT_ACCESS:
			response->result = MoveFileResponse::Result::CANNOT_ACCESS;
			break;
		case FileService::Result::FILE_EXISTS:
			response->result = MoveFileResponse::Result::FILE_EXISTS;
			break;
		case FileService::Result::CANNOT_WRITE:
			response->result = MoveFileResponse::Result::CANNOT_WRITE;
			break;
		case FileService::Result::FILE_OCCUPIED:
			response->result = MoveFileResponse::Result::FILE_OCCUPIED;
			break;
		}
	}
	return response;
}

CopyFileResponse* RequestController::handleRequest(QSslSocket* socket, const CopyFileRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new CopyFileResponse(request->id);
	if (!checkPath(request->dst) || !checkPath(request->src))
	{
		response->result = CopyFileResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto result = fileService.copyFile(request->dst, request->src, request->force);
		switch (result)
		{
		case FileService::Result::NO_SUCH_FILE:
			response->result = CopyFileResponse::Result::NO_SUCH_FILE;
			break;
		case FileService::Result::CANNOT_ACCESS:
			response->result = CopyFileResponse::Result::CANNOT_ACCESS;
			break;
		case FileService::Result::FILE_EXISTS:
			response->result = CopyFileResponse::Result::FILE_EXISTS;
			break;
		case FileService::Result::CANNOT_READ:
			response->result = CopyFileResponse::Result::CANNOT_READ;
			break;
		case FileService::Result::CANNOT_WRITE:
			response->result = CopyFileResponse::Result::CANNOT_WRITE;
			break;
		case FileService::Result::FILE_OCCUPIED:
			response->result = CopyFileResponse::Result::FILE_OCCUPIED;
			break;
		}
	}
	return response;
}

UploadFileResponse* RequestController::handleRequest(QSslSocket* socket, const UploadFileRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new UploadFileResponse(request->id);
	if (!checkPath(request->dst))
	{
		response->result = UploadFileResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto task = new FileDataTask;
		task->totalSize = request->size;
		auto result = fileService.createWriteFile(request->dst, task->file);
		switch (result)
		{
		case FileService::Result::CANNOT_ACCESS:
			response->result = UploadFileResponse::Result::CANNOT_ACCESS;
			return response;
		case FileService::Result::FILE_EXISTS:
			response->result = UploadFileResponse::Result::FILE_EXISTS;
			return response;
		}
		openedFilesMutex.lock();
		openedFiles.emplace(std::make_pair(socket, request->id), task);
		openedFilesMutex.unlock();
	}
	return response;
}

DownloadFileResponse* RequestController::handleRequest(QSslSocket* socket, const DownloadFileRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new DownloadFileResponse(request->id);
	if (!checkPath(request->src))
	{
		response->result = DownloadFileResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto task = new FileDataTask;
		auto result = fileService.openReadFile(request->src, task->file);
		switch (result)
		{
		case FileService::Result::CANNOT_ACCESS:
			response->result = DownloadFileResponse::Result::CANNOT_ACCESS;
			return response;
		case FileService::Result::CANNOT_READ:
			response->result = DownloadFileResponse::Result::CANNOT_READ;
			return response;
		}
		task->totalSize = task->file.size();
		response->size = task->file.size();
		openedFilesMutex.lock();
		openedFiles.emplace(std::make_pair(socket, request->id), task);
		openedFilesMutex.unlock();
	}
	return response;
}

RemoveFileResponse* RequestController::handleRequest(QSslSocket* socket, const RemoveFileRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new RemoveFileResponse(request->id);
	if (!checkPath(request->path))
	{
		response->result = RemoveFileResponse::Result::INVALID_ARGUMENT;
	}
	else
	{
		auto result = fileService.removeFile(request->path);
		switch (result)
		{
		case FileService::Result::NO_SUCH_FILE:
			response->result = RemoveFileResponse::Result::NO_SUCH_FILE;
			break;
		case FileService::Result::CANNOT_ACCESS:
			response->result = RemoveFileResponse::Result::CANNOT_ACCESS;
			break;
		case FileService::Result::FILE_OCCUPIED:
			response->result = RemoveFileResponse::Result::FILE_OCCUPIED;
			break;
		}
	}
	return response;
}

UploadDataResponse* RequestController::handleRequest(QSslSocket* socket, const UploadDataRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new UploadDataResponse(request->id);
	openedFilesMutex.lock();
	auto iter = openedFiles.find(std::make_pair(socket, request->parentId));
	if (iter == openedFiles.end())
	{
		response->result = UploadDataResponse::Result::INVALID_ARGUMENT;
		openedFilesMutex.unlock();
		return response;
	}
	auto& task = *iter->second;
	openedFilesMutex.unlock();

	// ??????
	task.mutex.lock();
	task.file.seek(request->offset);
	auto n = (int)task.file.write(request->data);
	if (n == request->data.size())
	{
		task.transferedSize += n;
		if (task.transferedSize >= task.totalSize)
		{
			assert(task.transferedSize == task.totalSize);
			task.file.close();
			task.mutex.unlock();
			openedFilesMutex.lock();
			openedFiles.erase(iter);
			openedFilesMutex.unlock();
			return response;
		}
	}
	else
	{
		response->result = UploadDataResponse::Result::CANNOT_WRITE;
	}
	task.mutex.unlock();
	return response;
}

DownloadDataResponse* RequestController::handleRequest(QSslSocket* socket, const DownloadDataRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new DownloadDataResponse(request->id);
	openedFilesMutex.lock();
	auto iter = openedFiles.find(std::make_pair(socket, request->parentId));
	if (iter == openedFiles.end())
	{
		response->result = DownloadDataResponse::Result::INVALID_ARGUMENT;
		openedFilesMutex.unlock();
		return response;
	}
	auto& task = *iter->second;
	openedFilesMutex.unlock();

	// ??????
	task.mutex.lock();
	task.file.seek(request->offset);
	response->data = task.file.read(request->size);
	if (response->data.size() == request->size)
	{
		task.transferedSize += request->size;
		if (task.transferedSize >= task.totalSize)
		{
			assert(task.transferedSize == task.totalSize);
			task.file.close();
			task.mutex.unlock();
			openedFilesMutex.lock();
			openedFiles.erase(iter);
			openedFilesMutex.unlock();
			return response;
		}
	}
	else
	{
		response->result = DownloadDataResponse::Result::CANNOT_READ;
	}
	task.mutex.unlock();
	return response;
}

CancelTransferResponse* RequestController::handleRequest(QSslSocket* socket, const CancelTransferRequest* request)
{
	if (!userService.isLogin(socket))
	{
		return nullptr;
	}
	auto response = new CancelTransferResponse(request->id);
	return response;
}

void RequestController::handleRequestConcurrent(QSslSocket* socket, QSharedPointer<Request> request)
{
	if (auto response = handleRequest(socket, request))
	{
		emit sendResponse(response);
	}
}

bool RequestController::checkPath(QString path)
{
	return QFileInfo(path).absoluteFilePath() == path;
}
