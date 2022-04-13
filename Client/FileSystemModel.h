#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"
#include "../SecureFileTransfer/FileService.h"

class FileSystemModel : public QAbstractTableModel
{
	Q_OBJECT

protected:
	explicit FileSystemModel(QObject *parent = nullptr);

public:
	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	QVariant getDisplayData(int row, int column) const;
	QVariant getTextAlignment(int column) const;

protected:
	QString openedDirectory;
	QVector<RemoteFileInfo> infoVec;
};

class LocalFileSystemModel : public FileSystemModel
{
	Q_OBJECT

public:
	explicit LocalFileSystemModel(QObject* parent = nullptr);

	void refresh();

private:
	FileService fileService;
};

class RemoteFileSystemModel : public FileSystemModel
{
	Q_OBJECT

public:
	explicit RemoteFileSystemModel(QObject* parent = nullptr)
		: FileSystemModel(parent) {}

	void checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);

private:
	int lastMessageId = 0;
};
