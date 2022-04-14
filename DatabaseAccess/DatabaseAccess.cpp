#include "stdafx.h"

#include "DatabaseAccess.h"
#include "User.h"

DatabaseAccess DatabaseAccess::instance;

DatabaseAccess::DatabaseAccess()
{
	qInfo() << u8"支持的数据库驱动" << QSqlDatabase::drivers();
}

bool DatabaseAccess::connectToDatabase()
{
    QMutexLocker locker(&mutex);
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    return db.open();
}

bool DatabaseAccess::init(QString path)
{
    QMutexLocker locker(&mutex);
    this->path = path;
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open())
    {
        return false;
    }
    // auto query = db.exec("drop table if exists [user]");
    auto tables =  db.tables();
    if (tables.empty())
    {
        qInfo() << u8"创建新数据库表";
        db.exec("create table [user]("
            "[id] integer primary key autoincrement,"
            "[username] varchar(255) not null unique,"
            "[password] binary(32) not null,"
            "[salt] binary(16) not null);");
    }
    db.close();
    return true;
}
