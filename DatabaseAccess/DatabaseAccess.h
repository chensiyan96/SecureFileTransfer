#pragma once

#include "databaseaccess_global.h"

#include "User.h"

struct DATABASEACCESS_EXPORT DatabaseAccess
{
    static DatabaseAccess instance;
    DatabaseAccess();
    bool connectToDatabase();
    bool init(QString path);
    QString path;
    QMutex mutex;
};
