#include "stdafx.h"

#include "FileService.h"

FileService::FileService(QObject *parent)
	: QObject(parent)
{
}

FileService::Result FileService::listFiles(QString directory, QVector<QString>& filenames)
{
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::makeDirectory(QString path)
{
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::createFile(QString path, quint64 size)
{
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::moveFile(QString dst, QString src)
{
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::copyFile(QString dst, QString src)
{
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::removeFile(QString path)
{
	return FileService::Result::SUCCESS;
}
