#pragma once

#include "securefiletransfer_global.h"

// 应用层协议报文格式定义的基类
struct SECUREFILETRANSFER_EXPORT AppLayerMessage
{
	enum class Type : quint8
	{
		// 账号管理
		REGISTER, LOGIN, LOGOUT, DESTROY_ACCOUNT, CHANGE_PASSWORD, 

		// 文件元数据管理		
		LIST_FILES, CHANGE_DIRECTORY, MAKE_DIRECTORY,
		MOVE_FILE, COPY_FILE, UPLOAD_FILE, DOWNLOAD_FILE, REMOVE_FILE,

		// 文件内容管理
		UPLOAD_DATA, DOWNLOAD_DATA, CANCEL_TRANSFER
	};

	AppLayerMessage(quint32 id, Type type) : id(id), type(type) {}
	virtual ~AppLayerMessage() = default;

	// 序列化成字节数组，然后可以写入TCP流
	virtual QByteArray serialize() const = 0;

	// 公共子过程：序列化报文头部
	char* serializeHeader(char* data, int size) const;

	const quint32 id; // 报文的序列号，用于匹配响应和请求
	const Type type; // 报文类型
};

// 请求报文，通常是客户端向服务器发送
struct SECUREFILETRANSFER_EXPORT Request : public AppLayerMessage
{
	// 从字节数组中反序列化构造对象
	static QSharedPointer<Request> deserialize(QByteArray src);

	Request(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

Q_DECLARE_METATYPE(QSharedPointer<Request>)

// 响应报文，通常是服务器向客户端发送
struct SECUREFILETRANSFER_EXPORT Response : public AppLayerMessage
{
	// 从字节数组中反序列化构造对象
	static QSharedPointer<Response> deserialize(QByteArray src);

	Response(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

Q_DECLARE_METATYPE(QSharedPointer<Response>)

// 用户注册请求
struct SECUREFILETRANSFER_EXPORT RegisterRequest : public Request
{
	RegisterRequest(quint32 id) : Request(id, Type::REGISTER) {}
	RegisterRequest(quint32 id, const char* data)
		: Request(id, Type::REGISTER) { deserialize(data); }

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString username;
	QByteArray password; // SHA256
};

// 用户注册响应
struct SECUREFILETRANSFER_EXPORT RegisterResponse : public Response
{
	RegisterResponse(quint32 id) : Response(id, Type::REGISTER) {}
	RegisterResponse(quint32 id, const char* data)
		: Response(id, Type::REGISTER) { deserialize(data); }

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, REPEAT_USERNAME
	} result = Result::SUCCESS;
};

// 用户登录请求
struct SECUREFILETRANSFER_EXPORT LoginRequest : public Request
{
	LoginRequest(quint32 id) : Request(id, Type::LOGIN) {}
	LoginRequest(quint32 id, const char* data)
		: Request(id, Type::LOGIN) { deserialize(data); }

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString username;
	QByteArray password; // SHA256
};

// 用户登录响应
struct SECUREFILETRANSFER_EXPORT LoginResponse : public Response
{
	LoginResponse(quint32 id) : Response(id, Type::LOGIN) {}
	LoginResponse(quint32 id, const char* data)
		: Response(id, Type::LOGIN) { deserialize(data); }

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, WRONG_USERNAME_OR_PASSWORD
	} result = Result::SUCCESS;
};
