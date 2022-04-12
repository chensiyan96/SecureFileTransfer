#pragma once

#include "securefiletransfer_global.h"

class SECUREFILETRANSFER_EXPORT FileService : public QObject
{
	Q_OBJECT

public:
	enum class Result
	{
		SUCCESS, NO_SUCH_FILE, NO_PERMISSION, CANNOT_WRITE, FILE_OCCUPIED, CONTENT_OCCUPIED
	};

public:
	explicit FileService(QObject* parent = nullptr);

	Result listFiles(QString directory, QVector<QString>& filenames);
	Result makeDirectory(QString path);
	Result createFile(QString path, quint64 size);
	Result moveFile(QString dst, QString src);
	Result copyFile(QString dst, QString src);
	Result removeFile(QString path);

	Result readFile(quint64 offset);
	Result writeFile(quint64 offset);
};
