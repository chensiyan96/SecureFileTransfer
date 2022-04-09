#include "stdafx.h"

#include "DatabaseAccess.h"

DatabaseAccess::DatabaseAccess()
{
	qDebug() << QSqlDatabase::drivers();

	auto db = QSqlDatabase::addDatabase("QSQLITE");

    //�������ݿ�
    db.setDatabaseName("D:/info.db");    //ʹ���ĸ����ݿ�

    //�����ݿ�
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

    query.prepare("insert into [user]([username], [password], [salt]) values(?, ?, ?);");
    query.addBindValue(QString("root"));
    query.addBindValue(QByteArray(32, Qt::Uninitialized));
    query.addBindValue(QByteArray(16, Qt::Uninitialized));
    query.exec();
    qDebug() << query.lastError();

    query.exec("select * from [user]");
    qDebug() << query.lastError();

    while (query.next()) //һ��һ�б���
    {
        qDebug() << query.value(0).toInt();
        qDebug() << query.value(1).toString();
        qDebug() << query.value(2).toByteArray();
        qDebug() << query.value(3).toByteArray();
    }
}
