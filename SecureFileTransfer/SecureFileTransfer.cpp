#include "stdafx.h"

#include "SecureFileTransfer.h"
#include "AppLayerMessage.h"

void SecureFileTransfer::testServer()
{
	// 如果不支持openssl，请看此博客
	// https://blog.csdn.net/YinShiJiaW/article/details/106220932
	qDebug() << u8"OpenSSL支持情况:" << QSslSocket::supportsSsl();
	qDebug() << u8"OpenSSL版本:" << QSslSocket::sslLibraryBuildVersionString();
}
