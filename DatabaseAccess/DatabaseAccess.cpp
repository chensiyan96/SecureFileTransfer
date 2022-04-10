#include "stdafx.h"

#include "DatabaseAccess.h"
#include "User.h"

DatabaseAccess::DatabaseAccess()
{
	qDebug() << QSqlDatabase::drivers();

	auto db = QSqlDatabase::addDatabase("QSQLITE");

    //连接数据库
    db.setDatabaseName("D:/info.db");    //使用哪个数据库

    //打开数据库
    if (!db.open())
    {
        return;
    }

    qDebug() << db.isOpen();

    auto query = db.exec("drop table if exists [user]");
    qDebug() << db.tables();

    query.exec("create table [user]("
        "[id] integer primary key autoincrement,"
        "[username] varchar(255) not null unique,"
        "[password] binary(32) not null,"
        "[salt] binary(16) not null);");
    qDebug() << db.tables();
    qDebug() << query.lastError();

    dao::User user;
    user.username = QString("root");
    user.password = QByteArray(32, Qt::Uninitialized);
    user.salt = QByteArray(16, Qt::Uninitialized);
    dao::insertUser(user);

    dao::selectUserByName("root", user);
    dao::updateUserPassword(user);
    dao::deleteUserById(user.id);
}
