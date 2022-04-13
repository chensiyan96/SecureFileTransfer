#pragma once

#include "securefiletransfer_global.h"

#include "RemoteFileInfo.h"

// 应用层协议报文格式定义的基类
struct SECUREFILETRANSFER_EXPORT AppLayerMessage
{
	enum class Type : quint8
	{
		// 账号管理
		REGISTER, LOGIN, LOGOUT, DELETE_USER,

		// 文件元数据管理
		LIST_FILES, MAKE_DIRECTORY,
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

	const quint32 id; // 请求的序列号，用于匹配响应和请求
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
		SUCCESS, INVALID_ARGUMENT, WRONG_USERNAME_OR_PASSWORD, LOGOUT_FIRST
	} result = Result::SUCCESS;
};

// 用户注销（登出）请求
struct SECUREFILETRANSFER_EXPORT LogoutRequest : public Request
{
	LogoutRequest(quint32 id) : Request(id, Type::LOGOUT) {}
	LogoutRequest(quint32 id, const char* data)
		: Request(id, Type::LOGOUT) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);
};

// 用户注销（登出）响应
struct SECUREFILETRANSFER_EXPORT LogoutResponse : public Response
{
	LogoutResponse(quint32 id) : Response(id, Type::LOGOUT) {}
	LogoutResponse(quint32 id, const char* data)
		: Response(id, Type::LOGOUT) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, DID_NOT_LOGIN
	} result = Result::SUCCESS;
};

// 删除用户请求
struct SECUREFILETRANSFER_EXPORT DeleteUserRequest : public Request
{
	DeleteUserRequest(quint32 id) : Request(id, Type::DELETE_USER) {}
	DeleteUserRequest(quint32 id, const char* data)
		: Request(id, Type::DELETE_USER) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);
};

// 删除用户响应
struct SECUREFILETRANSFER_EXPORT DeleteUserResponse : public Response
{
	DeleteUserResponse(quint32 id) : Response(id, Type::DELETE_USER) {}
	DeleteUserResponse(quint32 id, const char* data)
		: Response(id, Type::DELETE_USER) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, DID_NOT_LOGIN
	} result = Result::SUCCESS;
};

// 列出文件请求
struct SECUREFILETRANSFER_EXPORT ListFilesRequest : public Request
{
	ListFilesRequest(quint32 id) : Request(id, Type::LIST_FILES) {}
	ListFilesRequest(quint32 id, const char* data)
		: Request(id, Type::LIST_FILES) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString directory;
};

// 列出文件响应
struct SECUREFILETRANSFER_EXPORT ListFilesResponse : public Response
{
	ListFilesResponse(quint32 id) : Response(id, Type::LIST_FILES) {}
	ListFilesResponse(quint32 id, const char* data)
		: Response(id, Type::LIST_FILES) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, NO_SUCH_FILE, CANNOT_ACCESS
	} result = Result::SUCCESS;

	QVector<RemoteFileInfo> infoVec;
};

// 创建目录请求
struct SECUREFILETRANSFER_EXPORT MakeDirectoryRequest : public Request
{
	MakeDirectoryRequest(quint32 id) : Request(id, Type::MAKE_DIRECTORY) {}
	MakeDirectoryRequest(quint32 id, const char* data)
		: Request(id, Type::MAKE_DIRECTORY) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString path;
};

// 创建目录响应
struct SECUREFILETRANSFER_EXPORT MakeDirectoryResponse : public Response
{
	MakeDirectoryResponse(quint32 id) : Response(id, Type::MAKE_DIRECTORY) {}
	MakeDirectoryResponse(quint32 id, const char* data)
		: Response(id, Type::MAKE_DIRECTORY) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, CANNOT_ACCESS, FILE_EXISTS
	} result = Result::SUCCESS;
};

// 移动文件请求
struct SECUREFILETRANSFER_EXPORT MoveFileRequest : public Request
{
	MoveFileRequest(quint32 id) : Request(id, Type::MOVE_FILE) {}
	MoveFileRequest(quint32 id, const char* data)
		: Request(id, Type::MOVE_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString dst;
	QString src;
	bool force = false; // 是否强制覆盖目标文件
};

// 移动文件响应
struct SECUREFILETRANSFER_EXPORT MoveFileResponse : public Response
{
	MoveFileResponse(quint32 id) : Response(id, Type::MOVE_FILE) {}
	MoveFileResponse(quint32 id, const char* data)
		: Response(id, Type::MOVE_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, NO_SUCH_FILE, CANNOT_ACCESS, FILE_EXISTS, CANNOT_WRITE, FILE_OCCUPIED
	} result = Result::SUCCESS;
};

// 复制文件请求
struct SECUREFILETRANSFER_EXPORT CopyFileRequest : public Request
{
	CopyFileRequest(quint32 id) : Request(id, Type::COPY_FILE) {}
	CopyFileRequest(quint32 id, const char* data)
		: Request(id, Type::COPY_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString dst;
	QString src;
	bool force = false; // 是否强制覆盖目标文件
};

// 复制文件响应
struct SECUREFILETRANSFER_EXPORT CopyFileResponse : public Response
{
	CopyFileResponse(quint32 id) : Response(id, Type::COPY_FILE) {}
	CopyFileResponse(quint32 id, const char* data)
		: Response(id, Type::COPY_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, NO_SUCH_FILE, CANNOT_ACCESS, FILE_EXISTS, CANNOT_READ, CANNOT_WRITE, FILE_OCCUPIED
	} result = Result::SUCCESS;
};

// 上传文件请求
struct SECUREFILETRANSFER_EXPORT UploadFileRequest : public Request
{
	UploadFileRequest(quint32 id) : Request(id, Type::UPLOAD_FILE) {}
	UploadFileRequest(quint32 id, const char* data)
		: Request(id, Type::UPLOAD_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString dst;
	bool force = false; // 是否强制覆盖目标文件
};

// 上传文件响应
struct SECUREFILETRANSFER_EXPORT UploadFileResponse : public Response
{
	UploadFileResponse(quint32 id) : Response(id, Type::UPLOAD_FILE) {}
	UploadFileResponse(quint32 id, const char* data)
		: Response(id, Type::UPLOAD_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, CANNOT_ACCESS, CANNOT_WRITE, FILE_EXISTS
	} result = Result::SUCCESS;
};

// 下载文件请求
struct SECUREFILETRANSFER_EXPORT DownloadFileRequest : public Request
{
	DownloadFileRequest(quint32 id) : Request(id, Type::DOWNLOAD_FILE) {}
	DownloadFileRequest(quint32 id, const char* data)
		: Request(id, Type::DOWNLOAD_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString src;
};

// 下载文件响应
struct SECUREFILETRANSFER_EXPORT DownloadFileResponse : public Response
{
	DownloadFileResponse(quint32 id) : Response(id, Type::DOWNLOAD_FILE) {}
	DownloadFileResponse(quint32 id, const char* data)
		: Response(id, Type::DOWNLOAD_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, CANNOT_ACCESS, CANNOT_READ
	} result = Result::SUCCESS;

	quint64 fileSize;
};

// 删除文件请求
struct SECUREFILETRANSFER_EXPORT RemoveFileRequest : public Request
{
	RemoveFileRequest(quint32 id) : Request(id, Type::REMOVE_FILE) {}
	RemoveFileRequest(quint32 id, const char* data)
		: Request(id, Type::REMOVE_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	QString path;
};

// 删除文件响应
struct SECUREFILETRANSFER_EXPORT RemoveFileResponse : public Response
{
	RemoveFileResponse(quint32 id) : Response(id, Type::REMOVE_FILE) {}
	RemoveFileResponse(quint32 id, const char* data)
		: Response(id, Type::REMOVE_FILE) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, NO_SUCH_FILE, CANNOT_ACCESS, FILE_OCCUPIED
	} result = Result::SUCCESS;
};

// 上传数据请求
struct SECUREFILETRANSFER_EXPORT UploadDataRequest : public Request
{
	UploadDataRequest(quint32 id) : Request(id, Type::UPLOAD_DATA) {}
	UploadDataRequest(quint32 id, const char* data)
		: Request(id, Type::UPLOAD_DATA) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	quint64 offset;
	quint32 parentId;
	QByteArray data;
};

// 上传数据响应
struct SECUREFILETRANSFER_EXPORT UploadDataResponse : public Response
{
	UploadDataResponse(quint32 id) : Response(id, Type::UPLOAD_DATA) {}
	UploadDataResponse(quint32 id, const char* data)
		: Response(id, Type::UPLOAD_DATA) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, CANNOT_WRITE
	} result = Result::SUCCESS;
};

// 下载数据请求
struct SECUREFILETRANSFER_EXPORT DownloadDataRequest : public Request
{
	DownloadDataRequest(quint32 id) : Request(id, Type::DOWNLOAD_DATA) {}
	DownloadDataRequest(quint32 id, const char* data)
		: Request(id, Type::DOWNLOAD_DATA) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	quint64 offset;
	quint32 parentId;
	quint32 size;
};

// 下载数据响应
struct SECUREFILETRANSFER_EXPORT DownloadDataResponse : public Response
{
	DownloadDataResponse(quint32 id) : Response(id, Type::DOWNLOAD_DATA) {}
	DownloadDataResponse(quint32 id, const char* data)
		: Response(id, Type::DOWNLOAD_DATA) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT, CANNOT_READ
	} result = Result::SUCCESS;

	QByteArray data;
};

// 取消传输请求
struct SECUREFILETRANSFER_EXPORT CancelTransferRequest : public Request
{
	CancelTransferRequest(quint32 id) : Request(id, Type::CANCEL_TRANSFER) {}
	CancelTransferRequest(quint32 id, const char* data)
		: Request(id, Type::CANCEL_TRANSFER) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	quint32 parentId;
};

// 取消传输响应
struct SECUREFILETRANSFER_EXPORT CancelTransferResponse : public Response
{
	CancelTransferResponse(quint32 id) : Response(id, Type::CANCEL_TRANSFER) {}
	CancelTransferResponse(quint32 id, const char* data)
		: Response(id, Type::CANCEL_TRANSFER) {
		deserialize(data);
	}

	QByteArray serialize() const override;
	void deserialize(const char* data);

	enum class Result : quint8
	{
		SUCCESS, INVALID_ARGUMENT
	} result = Result::SUCCESS;
};
