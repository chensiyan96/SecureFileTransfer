#pragma once

#include "securefiletransfer_global.h"

// Ӧ�ò�Э�鱨�ĸ�ʽ����Ļ���
struct SECUREFILETRANSFER_EXPORT AppLayerMessage
{
	enum class Type : quint8
	{
		// �˺Ź���
		REGISTER, LOGIN, LOGOUT, DESTROY_ACCOUNT, CHANGE_PASSWORD, 

		// �ļ�Ԫ���ݹ���		
		LIST_FILES, CHANGE_DIRECTORY, MAKE_DIRECTORY,
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

	const quint32 id; // ���ĵ����кţ�����ƥ����Ӧ������
	const Type type; // ��������
};

// �����ģ�ͨ���ǿͻ��������������
struct SECUREFILETRANSFER_EXPORT Request : public AppLayerMessage
{
	// ���ֽ������з����л��������
	static Request* deserialize(QByteArray src);

	Request(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

// ��Ӧ���ģ�ͨ���Ƿ�������ͻ��˷���
struct SECUREFILETRANSFER_EXPORT Response : public AppLayerMessage
{
	// ���ֽ������з����л��������
	static Response* deserialize(QByteArray src);

	Response(quint32 id, Type type) : AppLayerMessage(id, type) {}
};

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
		SUCCESS, INVALID_ARGUMENT, WRONG_USERNAME_OR_PASSWORD
	} result = Result::SUCCESS;
};
