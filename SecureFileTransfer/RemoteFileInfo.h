#pragma once

struct RemoteFileInfo
{
	QString fileName;
	QDateTime lastModified;
	quint64 size = 0;
	bool isDirectory = false;
	bool isReadable = false;
	bool isWritable = false;
};
