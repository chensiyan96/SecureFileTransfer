#pragma once

#include "securefiletransfer_global.h"

#include "RemoteFileInfo.h"

// Ӧ�ò�Э�鱨�ĸ�ʽ����Ļ���
struct SECUREFILETRANSFER_EXPORT AppLayerMessage
{
	enum class Type : quint8
	{
		// �˺Ź���
		REGISTER, LOGIN, LOGOUT, DELETE_USER,

		// �ļ�Ԫ���ݹ���
		LIST_FILES, MAKE_DIRECTORY,
		MOVE_FILE, COPY_FILE, UPLOAD_FILE, DOWNLOAD_FILE, REMOVE_FILE,

		// �ļ����ݹ���
		UPLOAD_DATA, DOWNLOAD_DATA, CANCEL_TRANSFER
	};

	AppLayerMessage(quint32 id, Type type) : id(id), type(type) {}
	virtual ~AppLayerMessage() = default;

	// ���л����ֽ����飬Ȼ�����д��TCP��
	virtual QByteArray serialize() const = 0;

	// �����ӹ��̣����л�����ͷ��
	char* serializeHeader(char* data, int size) const;

	const quint32 id; // ��������кţ�����ƥ����Ӧ������
	const Type type; // ��������
};

// �����ģ�ͨ���ǿͻ��������������
struct SECUREFILETRANSFER_EXPORT Request : public AppLayerMessage
{
	// ���ֽ������з����л��������
	static QSharedPointer<Request> deserialize(QByteArray src);

	Request(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

Q_DECLARE_METATYPE(QSharedPointer<Request>)

// ��Ӧ���ģ�ͨ���Ƿ�������ͻ��˷���
struct SECUREFILETRANSFER_EXPORT Response : public AppLayerMessage
{
	// ���ֽ������з����л��������
	static QSharedPointer<Response> deserialize(QByteArray src);

	Response(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

Q_DECLARE_METATYPE(QSharedPointer<Response>)

// �û�ע������
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

// �û�ע����Ӧ
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

// �û���¼����
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

// �û���¼��Ӧ
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

// �û�ע�����ǳ�������
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

// �û�ע�����ǳ�����Ӧ
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

// ɾ���û�����
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

// ɾ���û���Ӧ
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

// �г��ļ�����
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

// �г��ļ���Ӧ
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

// ����Ŀ¼����
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

// ����Ŀ¼��Ӧ
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

// �ƶ��ļ�����
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
	bool force = false; // �Ƿ�ǿ�Ƹ���Ŀ���ļ�
};

// �ƶ��ļ���Ӧ
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

// �����ļ�����
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
	bool force = false; // �Ƿ�ǿ�Ƹ���Ŀ���ļ�
};

// �����ļ���Ӧ
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

// �ϴ��ļ�����
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
	bool force = false; // �Ƿ�ǿ�Ƹ���Ŀ���ļ�
};

// �ϴ��ļ���Ӧ
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

// �����ļ�����
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

// �����ļ���Ӧ
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

// ɾ���ļ�����
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

// ɾ���ļ���Ӧ
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

// �ϴ���������
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

// �ϴ�������Ӧ
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

// ������������
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

// ����������Ӧ
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

// ȡ����������
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

// ȡ��������Ӧ
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
