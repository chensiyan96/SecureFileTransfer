#pragma once

#include "databaseaccess_global.h"

namespace dao
{
	struct User
	{
		int id = 0; // ����
		QString username;
		QByteArray password; // SHA256(SHA256, salt)
		QByteArray salt; // 16�ֽ�
	};

	bool DATABASEACCESS_EXPORT insertUser(User& user);
	bool DATABASEACCESS_EXPORT selectUserById(int id, User& user);
	bool DATABASEACCESS_EXPORT selectUserByName(QString username, User& user);
	bool DATABASEACCESS_EXPORT deleteUserById(int id);
	bool DATABASEACCESS_EXPORT updateUserPassword(const User& user);
}
