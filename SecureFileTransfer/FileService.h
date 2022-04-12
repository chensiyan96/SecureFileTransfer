#pragma once

#include "securefiletransfer_global.h"

#include "RemoteFileInfo.h"

class SECUREFILETRANSFER_EXPORT FileService : public QObject
{
	Q_OBJECT

public:
	enum class Result
	{
		SUCCESS, NO_SUCH_FILE, CANNOT_ACCESS, FILE_EXISTS,
		CANNOT_READ, CANNOT_WRITE, FILE_OCCUPIED
	};

public:
	explicit FileService(QObject* parent = nullptr);

	Result listFiles(QString directory, QVector<RemoteFileInfo>& infoVec);
	Result makeDirectory(QString path);
	Result createFile(QString path, QFile& file);
	Result moveFile(QString dst, QString src, bool force);
	Result copyFile(QString dst, QString src, bool force);
	Result removeFile(QString path);

	Result readFile(quint64 offset);
	Result writeFile(quint64 offset);

private:
	bool isAccessible(QString path);
	Result copyFileRecursive(QString dstDir, QString srcDir, bool force);

public:
	QVector<QString> accessibleDirectories;
	bool allDirectoriesAccessible = true;
};
