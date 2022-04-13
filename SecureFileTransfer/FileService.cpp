#include "stdafx.h"

#include "FileService.h"

FileService::FileService(QObject *parent)
	: QObject(parent)
{
	accessibleDirectories.append("D:/test");
}

FileService::Result FileService::listFiles(QString directory, QVector<RemoteFileInfo>& infoVec)
{
	if (!isAccessible(directory))
	{
		return FileService::Result::CANNOT_ACCESS;
	}
	QDir dir(directory);
	if (!dir.exists())
	{
		return FileService::Result::NO_SUCH_FILE;
	}
	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::DirsFirst);
	auto infoList = dir.entryInfoList();
	infoVec.resize(infoList.size());
	auto iter = infoList.begin();
	for (auto& info : infoVec)
	{
		auto& file = *iter;
		info.fileName = file.fileName();
		info.lastModified = file.lastModified();
		info.size = file.size();
		info.isDirectory = file.isDir();
		info.isReadable = file.isReadable();
		info.isWritable = file.isWritable();
		++iter;
	}
	return FileService::Result::SUCCESS;
}

FileService::Result FileService::makeDirectory(QString path)
{
	if (!isAccessible(path))
	{
		return FileService::Result::CANNOT_ACCESS;
	}
	if (QFile::exists(path))
	{
		return FileService::Result::FILE_EXISTS;
	}
	QDir dir;
	if (dir.mkpath(path))
	{
		return FileService::Result::SUCCESS;
	}
	return dir.exists() ? FileService::Result::FILE_EXISTS : FileService::Result::CANNOT_ACCESS;
}

FileService::Result FileService::createFile(QString path, QFile& file)
{
	if (!isAccessible(path))
	{
		return FileService::Result::CANNOT_ACCESS;
	}
	file.setFileName(path);
	if (file.exists())
	{
		return FileService::Result::FILE_EXISTS;
	}
	file.open(QIODevice::WriteOnly);
	if (file.isOpen())
	{
		return FileService::Result::SUCCESS;
	}
	return file.exists() ? FileService::Result::FILE_EXISTS : FileService::Result::CANNOT_ACCESS;
}

FileService::Result FileService::moveFile(QString dst, QString src, bool force)
{
	if (!isAccessible(dst) || !isAccessible(src))
	{
		return FileService::Result::CANNOT_ACCESS;
	}
	if (!QFile::exists(src))
	{
		return FileService::Result::NO_SUCH_FILE;
	}
	if (force)
	{
		if (!QFile(dst).isWritable())
		{
			return FileService::Result::CANNOT_WRITE;
		}
	}
	else
	{
		if (QFile::exists(dst))
		{
			return FileService::Result::FILE_EXISTS;
		}
	}
	if (QFile::rename(src, dst))
	{
		return FileService::Result::SUCCESS;
	}
	return FileService::Result::FILE_OCCUPIED;
}

FileService::Result FileService::copyFile(QString dst, QString src, bool force)
{
	if (!isAccessible(dst) || !isAccessible(src))
	{
		return FileService::Result::CANNOT_ACCESS;
	}
	if (!QFile::exists(src))
	{
		return FileService::Result::NO_SUCH_FILE;
	}
	if (!QFile(src).isReadable())
	{
		return FileService::Result::CANNOT_READ;
	}
	if (force)
	{
		if (!QFile(dst).isWritable())
		{
			return FileService::Result::CANNOT_WRITE;
		}
	}
	else
	{
		if (QFile::exists(dst))
		{
			return FileService::Result::FILE_EXISTS;
		}
	}
	QFileInfo info(src);
	if (info.isDir())
	{
		return copyFileRecursive(dst, src, force);
	}
	else
	{
		if (QFile::copy(src, dst))
		{
			return FileService::Result::SUCCESS;
		}
	}
	return FileService::Result::FILE_OCCUPIED;
}

FileService::Result FileService::removeFile(QString path)
{
	if (!QFile::exists(path))
	{
		return FileService::Result::NO_SUCH_FILE;
	}
	QFileInfo info(path);
	if (info.isDir())
	{
		QDir dir(path);
		if (dir.removeRecursively())
		{
			return FileService::Result::SUCCESS;
		}
	}
	else
	{
		QFile file(path);
		if (file.remove())
		{
			return FileService::Result::SUCCESS;
		}
	}
	return FileService::Result::FILE_OCCUPIED;
}

FileService::Result FileService::readFile(QFile& file, quint64 offset, QByteArray& data)
{
	file.seek(offset);
	auto n = (int)file.read(data.data(), data.size());
	return n == data.size() ? FileService::Result::SUCCESS : FileService::Result::CANNOT_READ;
}

FileService::Result FileService::writeFile(QFile& file, quint64 offset, const QByteArray& data)
{
	file.seek(offset);
	auto n = (int)file.write(data.constData(), data.size());
	return n == data.size() ? FileService::Result::SUCCESS : FileService::Result::CANNOT_WRITE;
}

bool FileService::isAccessible(QString path)
{
	if (allDirectoriesAccessible)
	{
		return true;
	}
	for (auto directory : accessibleDirectories)
	{
		if (path.indexOf(directory) == 0)
		{
			return true;
		}
	}
	return false;
}

FileService::Result FileService::copyFileRecursive(QString dstDir, QString srcDir, bool force)
{
	QVector<RemoteFileInfo> infoVec;
	listFiles(srcDir, infoVec);
	QDir dir;
	if (!dir.exists() && !dir.mkpath(dstDir))
	{
		return FileService::Result::FILE_EXISTS;
	}
	for (auto& info : infoVec)
	{
		auto dst = dstDir + "/" + info.fileName;
		auto src = srcDir + "/" + info.fileName;
		if (!QFile::exists(src))
		{
			continue;
		}
		if (!force && QFile::exists(dst))
		{
			continue;
		}
		QFileInfo info(src);
		if (info.isDir())
		{
			copyFileRecursive(dst, src, force);
		}
		else
		{
			QFile::copy(src, dst);
		}
	}
	return FileService::Result::SUCCESS;
}
