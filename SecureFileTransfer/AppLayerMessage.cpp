#include "stdafx.h"

#include "AppLayerMessage.h"

Request* Request::deserialize(QByteArray src)
{
	auto data = src.constData();
	auto id = *reinterpret_cast<const quint32*>(data);
	switch (*reinterpret_cast<const Type*>(data + 4))
	{
	case Type::REGISTER:
		return new RegisterRequest(id, data + 5);
	case Type::LOGIN:
		return new LoginRequest(id, data + 5);
	default:
		return nullptr;
	}
}

Response* Response::deserialize(QByteArray src)
{
	auto data = src.constData();
	auto id = *reinterpret_cast<const quint32*>(data);
	switch (*reinterpret_cast<const Type*>(data + 4))
	{
	case Type::REGISTER: 
		return new RegisterResponse(id, data + 5);
	case Type::LOGIN:
		return new LoginResponse(id, data + 5);
	default:
		return nullptr;
	}
}

QByteArray RegisterRequest::serialize() const
{
	auto username_utf8 = username.toUtf8();
	QByteArray result(4 + 1 + username_utf8.size() + 1 + password.size(), Qt::Uninitialized);
	auto data = result.data();
	*reinterpret_cast<quint32*>(data) = id;
	data += 4; // idռ4�ֽ�
	*reinterpret_cast<Type*>(data) = type;
	data += 1; // typeռ1�ֽ�
	memcpy_s(data, username_utf8.size(), username_utf8.constData(), username_utf8.size());
	data += username_utf8.size(); // �ɱ䳤�ȵ��û���
	*data = '\0'; // ��'\0'��β
	data += 1;
	memcpy_s(data, password.size(), password.constData(), password.size()); // 32�ֽڵ�SHA256
	return result;
}

void RegisterRequest::deserialize(const char* data)
{
	username.fromUtf8(data);
	while (*data++); // �ɱ䳤�ȵ��û���
	password.resize(32);
	memcpy_s(password.data(), password.size(), data, password.size()); // 32�ֽڵ�SHA256
}

QByteArray RegisterResponse::serialize() const
{
	return QByteArray();
}

void RegisterResponse::deserialize(const char* data)
{
}

QByteArray LoginRequest::serialize() const
{
	return QByteArray();
}

void LoginRequest::deserialize(const char* data)
{
}

QByteArray LoginResponse::serialize() const
{
	return QByteArray();
}

void LoginResponse::deserialize(const char* data)
{
}
