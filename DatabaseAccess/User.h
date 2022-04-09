#pragma once

namespace dao
{
	struct User
	{
		int id; // Ö÷¼ü
		QString username;
		QByteArray password; // SHA256(SHA256, salt)
		QByteArray salt; // 16×Ö½Ú
	};

	bool insertUser(User& user);
	bool selectUserById(quint32 id);
	bool selectUserByName(QString username);
	bool deleteUserById(quint32 id);
	bool updateUserById(quint32 id);
}
