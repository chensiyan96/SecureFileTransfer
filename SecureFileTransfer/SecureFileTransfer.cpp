#include "stdafx.h"

#include "SecureFileTransfer.h"
#include "AppLayerMessage.h"

void SecureFileTransfer::testServer()
{
	// �����֧��openssl���뿴�˲���
	// https://blog.csdn.net/YinShiJiaW/article/details/106220932
	qDebug() << u8"OpenSSL֧�����:" << QSslSocket::supportsSsl();
	qDebug() << u8"OpenSSL�汾:" << QSslSocket::sslLibraryBuildVersionString();
}
