#include "stdafx.h"

#include "User.h"

#define CHECK_QUERY_ERROR(query, error) \
    auto error = query.lastError(); \
    if (error.isValid()) \
    { \
        qInfo() << query.lastQuery() << error; \
        return false; \
    }

static bool getNextUserFromQuery(QSqlQuery& query, dao::User & user)
{
    if (query.next())
    {
        user.id = query.value(0).toInt();
        user.username = query.value(1).toString();
        user.password = query.value(2).toByteArray();
        user.salt = query.value(3).toByteArray();
        return true;
    }
    return false;
}

bool dao::insertUser(User& user)
{
    QSqlQuery query;
    query.prepare("insert into [user]([username], [password], [salt]) values(?, ?, ?);");
    query.addBindValue(user.username);
    query.addBindValue(user.password);
    query.addBindValue(user.salt);
    query.exec();
    CHECK_QUERY_ERROR(query, error);
    user.id = query.lastInsertId().toInt();
    return true;
}

bool dao::selectUserById(int id, User& user)
{
    QSqlQuery query;
    query.prepare("select * from [user] where [id] = ?");
    query.addBindValue(id);
    query.exec();
    CHECK_QUERY_ERROR(query, error);
    return getNextUserFromQuery(query, user);
}

bool dao::selectUserByName(QString username, User& user)
{
    QSqlQuery query;
    query.prepare("select * from [user] where [username] = ?");
    query.addBindValue(username);
    query.exec();
    CHECK_QUERY_ERROR(query, error);
    return getNextUserFromQuery(query, user);
}

bool dao::deleteUserById(int id)
{
    QSqlQuery query;
    query.prepare("delete from [user] where [id] = ?");
    query.addBindValue(id);
    query.exec();
    CHECK_QUERY_ERROR(query, error);
    return query.numRowsAffected() > 0;
}

bool dao::updateUserPassword(const User& user)
{
    QSqlQuery query;
    query.prepare("update [user] set [password] = ? where [id] = ?");
    query.addBindValue(user.password);
    query.addBindValue(user.id);
    query.exec();
    CHECK_QUERY_ERROR(query, error);
    return query.numRowsAffected() > 0;
}
