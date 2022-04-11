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
