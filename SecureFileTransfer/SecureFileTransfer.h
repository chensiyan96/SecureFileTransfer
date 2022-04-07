#pragma once

#include "securefiletransfer_global.h"

class SECUREFILETRANSFER_EXPORT SecureFileTransfer : public QObject
{
    Q_OBJECT

public:
    void testServer();
    void testClient();

private:
    void newConnectionSlot();
};
