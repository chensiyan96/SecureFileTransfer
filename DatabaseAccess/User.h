#pragma once

namespace dao
{
	struct User
	{
		int id; // ����
		QString username;
		QByteArray password; // SHA256(SHA256, salt)
		QByteArray salt; // 16�ֽ�
	};

	bool insertUser(User& user);
	bool selectUserById(quint32 id);
	bool selectUserByName(QString username);
	bool deleteUserById(quint32 id);
	bool updateUserById(quint32 id);
}
