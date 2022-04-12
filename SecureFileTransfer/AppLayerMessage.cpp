#include "stdafx.h"

#include "AppLayerMessage.h"

char* AppLayerMessage::serializeHeader(char* data, int size) const
{
	*reinterpret_cast<quint32*>(data) = id;
	data += 4; // id占4字节
	*reinterpret_cast<int*>(data) = size;
	data += 3; // 报文总长度占3字节
	*reinterpret_cast<Type*>(data) = type;
	data += 1; // 报文类型占1字节
	return data;
}

QSharedPointer<Request> Request::deserialize(QByteArray src)
{
	auto data = src.constData();
	auto id = *reinterpret_cast<const quint32*>(data);
	switch (*reinterpret_cast<const Type*>(data + 7))
	{
	case Type::REGISTER:
		return QSharedPointer<Request>(new RegisterRequest(id, data + 8));
	case Type::LOGIN:
		return QSharedPointer<Request>(new LoginRequest(id, data + 8));
	case Type::LOGOUT:
		return QSharedPointer<Request>(new LogoutRequest(id, data + 8));
	case Type::DELETE_USER:
		return QSharedPointer<Request>(new DeleteUserRequest(id, data + 8));
	case Type::LIST_FILES:
		return QSharedPointer<Request>(new ListFilesRequest(id, data + 8));
	case Type::MAKE_DIRECTORY:
		return QSharedPointer<Request>(new MakeDirectoryRequest(id, data + 8));
	case Type::MOVE_FILE:
		return QSharedPointer<Request>(new MoveFileRequest(id, data + 8));
	case Type::COPY_FILE:
		return QSharedPointer<Request>(new CopyFileRequest(id, data + 8));
	case Type::UPLOAD_FILE:
		return QSharedPointer<Request>(new UploadFileRequest(id, data + 8));
	case Type::DOWNLOAD_FILE:
		return QSharedPointer<Request>(new DownloadFileRequest(id, data + 8));
	case Type::REMOVE_FILE:
		return QSharedPointer<Request>(new RemoveFileRequest(id, data + 8));
	case Type::UPLOAD_DATA:
		return QSharedPointer<Request>(new UploadDataRequest(id, data + 8));
	case Type::DOWNLOAD_DATA:
		return QSharedPointer<Request>(new DownloadDataRequest(id, data + 8));
	case Type::CANCEL_TRANSFER:
		return QSharedPointer<Request>(new CancelTransferRequest(id, data + 8));
	default:
		return nullptr;
	}
}

QSharedPointer<Response> Response::deserialize(QByteArray src)
{
	auto data = src.constData();
	auto id = *reinterpret_cast<const quint32*>(data);
	switch (*reinterpret_cast<const Type*>(data + 7))
	{
	case Type::REGISTER: 
		return QSharedPointer<Response>(new RegisterResponse(id, data + 8));
	case Type::LOGIN:
		return QSharedPointer<Response>(new LoginResponse(id, data + 8));
	case Type::LOGOUT:
		return QSharedPointer<Response>(new LogoutResponse(id, data + 8));
	case Type::DELETE_USER:
		return QSharedPointer<Response>(new DeleteUserResponse(id, data + 8));
	case Type::LIST_FILES:
		return QSharedPointer<Response>(new ListFilesResponse(id, data + 8));
	case Type::MAKE_DIRECTORY:
		return QSharedPointer<Response>(new MakeDirectoryResponse(id, data + 8));
	case Type::MOVE_FILE:
		return QSharedPointer<Response>(new MoveFileResponse(id, data + 8));
	case Type::COPY_FILE:
		return QSharedPointer<Response>(new CopyFileResponse(id, data + 8));
	case Type::UPLOAD_FILE:
		return QSharedPointer<Response>(new UploadFileResponse(id, data + 8));
	case Type::DOWNLOAD_FILE:
		return QSharedPointer<Response>(new DownloadFileResponse(id, data + 8));
	case Type::REMOVE_FILE:
		return QSharedPointer<Response>(new RemoveFileResponse(id, data + 8));
	case Type::UPLOAD_DATA:
		return QSharedPointer<Response>(new UploadDataResponse(id, data + 8));
	case Type::DOWNLOAD_DATA:
		return QSharedPointer<Response>(new DownloadDataResponse(id, data + 8));
	case Type::CANCEL_TRANSFER:
		return QSharedPointer<Response>(new CancelTransferResponse(id, data + 8));
	default:
		return nullptr;
	}
}

QByteArray RegisterRequest::serialize() const
{
	auto username_utf8 = username.toUtf8();
	QByteArray result(8 + password.size() + username_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, password.size(), password.constData(), password.size());
	data += password.size(); // 密码是32字节的SHA256
	memcpy_s(data, username_utf8.size(), username_utf8.constData(), username_utf8.size());
	data += username_utf8.size(); // 可变长度的用户名
	*data = '\0'; // 以'\0'结尾
	return result;
}

void RegisterRequest::deserialize(const char* data)
{
	password.resize(32);
	memcpy_s(password.data(), password.size(), data, password.size());
	data += password.size(); // 密码是32字节的SHA256
	username = QString(data);
}

QByteArray RegisterResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void RegisterResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray LoginRequest::serialize() const
{
	auto username_utf8 = username.toUtf8();
	QByteArray result(8 + password.size() + username_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, password.size(), password.constData(), password.size());
	data += password.size(); // 密码是32字节的SHA256
	memcpy_s(data, username_utf8.size(), username_utf8.constData(), username_utf8.size());
	data += username_utf8.size(); // 可变长度的用户名
	*data = '\0'; // 以'\0'结尾
	return result;
}

void LoginRequest::deserialize(const char* data)
{
	password.resize(32);
	memcpy_s(password.data(), password.size(), data, password.size());
	data += password.size(); // 密码是32字节的SHA256
	username = QString(data);
}

QByteArray LoginResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void LoginResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray LogoutRequest::serialize() const
{
	QByteArray result(8, Qt::Uninitialized);
	serializeHeader(result.data(), result.size());
	return result;
}

void LogoutRequest::deserialize(const char* data)
{
}

QByteArray LogoutResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void LogoutResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray DeleteUserRequest::serialize() const
{
	QByteArray result(8, Qt::Uninitialized);
	serializeHeader(result.data(), result.size());
	return result;
}

void DeleteUserRequest::deserialize(const char* data)
{
}

QByteArray DeleteUserResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void DeleteUserResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray ListFilesRequest::serialize() const
{
	auto directory_utf8 = directory.toUtf8();
	QByteArray result(8 + directory_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, directory_utf8.size(), directory_utf8.constData(), directory_utf8.size());
	data += directory_utf8.size();
	*data = '\0';
	return result;
}

void ListFilesRequest::deserialize(const char* data)
{
	directory = QString(data);
}

QByteArray ListFilesResponse::serialize() const
{
	QByteArray result(11, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	data += 1;
	auto num = quint16(filenames.size() & 0xffff);
	*reinterpret_cast<quint16*>(data) = num;
	for (auto name : filenames)
	{
		result.append(name);
		result.append('\0');
	}
	return result;
}

void ListFilesResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
	data += 1;
	int num = (int)*reinterpret_cast<const quint16*>(data);
	data += 2;
	filenames.resize(num);
	for (int i = 0; i < num; i++)
	{
		filenames[i] = QString(data);
		while (*data++);
	}
}

QByteArray MakeDirectoryRequest::serialize() const
{
	auto path_utf8 = path.toUtf8();
	QByteArray result(8 + path_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, path_utf8.size(), path_utf8.constData(), path_utf8.size());
	data += path_utf8.size();
	*data = '\0';
	return result;
}

void MakeDirectoryRequest::deserialize(const char* data)
{
	path = QString(data);
}

QByteArray MakeDirectoryResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void MakeDirectoryResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray MoveFileRequest::serialize() const
{
	auto dst_utf8 = dst.toUtf8();
	auto src_utf8 = src.toUtf8();
	QByteArray result(8 + dst_utf8.size() + 1 + src_utf8.size() + 1 + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, dst_utf8.size(), dst_utf8.constData(), dst_utf8.size());
	data += dst_utf8.size();
	*data = '\0';
	data += 1;
	memcpy_s(data, src_utf8.size(), src_utf8.constData(), src_utf8.size());
	data += src_utf8.size();
	*data = '\0';
	data += 1;
	*reinterpret_cast<bool*>(data) = force;
	return result;
}

void MoveFileRequest::deserialize(const char* data)
{
	dst = QString(data);
	while (*data++);
	src = QString(data);
	while (*data++);
	force = *reinterpret_cast<const bool*>(data);
}

QByteArray MoveFileResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void MoveFileResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray CopyFileRequest::serialize() const
{
	auto dst_utf8 = dst.toUtf8();
	auto src_utf8 = src.toUtf8();
	QByteArray result(8 + dst_utf8.size() + 1 + src_utf8.size() + 1 + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, dst_utf8.size(), dst_utf8.constData(), dst_utf8.size());
	data += dst_utf8.size();
	*data = '\0';
	data += 1;
	memcpy_s(data, src_utf8.size(), src_utf8.constData(), src_utf8.size());
	data += src_utf8.size();
	*data = '\0';
	data += 1;
	*reinterpret_cast<bool*>(data) = force;
	return result;
}

void CopyFileRequest::deserialize(const char* data)
{
	dst = QString(data);
	while (*data++);
	src = QString(data);
	while (*data++);
	force = *reinterpret_cast<const bool*>(data);
}

QByteArray CopyFileResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void CopyFileResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray UploadFileRequest::serialize() const
{
	auto dst_utf8 = dst.toUtf8();
	QByteArray result(8 + dst_utf8.size() + 1 + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, dst_utf8.size(), dst_utf8.constData(), dst_utf8.size());
	data += dst_utf8.size();
	*data = '\0';
	data += 1;
	*reinterpret_cast<bool*>(data) = force;
	return result;
}

void UploadFileRequest::deserialize(const char* data)
{
	dst = QString(data);
	while (*data++);
	force = *reinterpret_cast<const bool*>(data);
}

QByteArray UploadFileResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void UploadFileResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray DownloadFileRequest::serialize() const
{
	auto src_utf8 = src.toUtf8();
	QByteArray result(8 + src_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, src_utf8.size(), src_utf8.constData(), src_utf8.size());
	data += src_utf8.size();
	*data = '\0';
	return result;
}

void DownloadFileRequest::deserialize(const char* data)
{
	src = QString(data);
}

QByteArray DownloadFileResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void DownloadFileResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray RemoveFileRequest::serialize() const
{
	auto path_utf8 = path.toUtf8();
	QByteArray result(8 + path_utf8.size() + 1, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	memcpy_s(data, path_utf8.size(), path_utf8.constData(), path_utf8.size());
	data += path_utf8.size();
	*data = '\0';
	return result;
}

void RemoveFileRequest::deserialize(const char* data)
{
	path = QString(data);
}

QByteArray RemoveFileResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void RemoveFileResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray UploadDataRequest::serialize() const
{
	int size = this->data.size();
	QByteArray result(24 + size, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<quint64*>(data) = offset;
	data += 8;
	*reinterpret_cast<quint32*>(data) = parentId;
	data += 4;
	*reinterpret_cast<int*>(data) = size;
	data += 4;
	memcpy_s(data, size, this->data.constData(), size);
	return result;
}

void UploadDataRequest::deserialize(const char* data)
{
	offset = *reinterpret_cast<const quint64*>(data);
	data += 8;
	parentId = *reinterpret_cast<const quint32*>(data);
	data += 4;
	int size = *reinterpret_cast<const int*>(data);
	data += 4;
	memcpy_s(this->data.data(), size, data, size);
}

QByteArray UploadDataResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void UploadDataResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}

QByteArray DownloadDataRequest::serialize() const
{
	QByteArray result(24, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<quint64*>(data) = offset;
	data += 8;
	*reinterpret_cast<quint32*>(data) = parentId;
	data += 4;
	*reinterpret_cast<quint32*>(data) = size;
	return result;
}

void DownloadDataRequest::deserialize(const char* data)
{
	offset = *reinterpret_cast<const quint64*>(data);
	data += 8;
	parentId = *reinterpret_cast<const quint32*>(data);
	data += 4;
	size = *reinterpret_cast<const quint32*>(data);
}

QByteArray DownloadDataResponse::serialize() const
{
	int size = this->data.size();
	QByteArray result(13 + size, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	data += 1;
	*reinterpret_cast<int*>(data) = size;
	data += 4;
	memcpy_s(data, size, this->data.constData(), size);
	return result;
}

void DownloadDataResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
	data += 1;
	int size = *reinterpret_cast<const int*>(data);
	data += 4;
	this->data.resize(size);
	memcpy_s(this->data.data(), size, data, size);
}

QByteArray CancelTransferRequest::serialize() const
{
	QByteArray result(12, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<quint32*>(data) = parentId;
	return result;
}

void CancelTransferRequest::deserialize(const char* data)
{
	parentId = *reinterpret_cast<const quint32*>(data);
}

QByteArray CancelTransferResponse::serialize() const
{
	QByteArray result(9, Qt::Uninitialized);
	auto data = serializeHeader(result.data(), result.size());
	*reinterpret_cast<Result*>(data) = this->result;
	return result;
}

void CancelTransferResponse::deserialize(const char* data)
{
	result = *reinterpret_cast<const Result*>(data);
}
