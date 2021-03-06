#include "stdafx.h"

#include "FileSystemModel.h"
#include "NetworkController.h"

constexpr int NUM_COLUMNS = 5;

FileSystemModel::FileSystemModel(QObject *parent)
	: QAbstractTableModel(parent)
{
}

int FileSystemModel::rowCount(const QModelIndex& parent) const
{
	return infoVec.size();
}

int FileSystemModel::columnCount(const QModelIndex& parent) const
{
	return NUM_COLUMNS;
}

QVariant FileSystemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	switch (role)
	{
	case Qt::DisplayRole: // 单元格中数据
		return getDisplayData(index.row(), index.column());
	case Qt::TextAlignmentRole: // 对齐格式
		return getTextAlignment(index.column());
	default:
		return QVariant();
	}
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		static const QString s[NUM_COLUMNS] = { u8"名称", u8"大小", u8"类型", u8"访问权限", u8"修改时间"};
		return section >= 0 && section < NUM_COLUMNS ? s[section] : QVariant();
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}

QString FileSystemModel::getFileName(int row) const
{
	return infoVec[row].fileName;
}

QString FileSystemModel::getPath(QString fileName) const
{
	return openedDirectory + fileName;
}

void FileSystemModel::openDirectory(const QModelIndex& index)
{
	int row = index.row();
	if (infoVec[row].isDirectory)
	{
		openedDirectory = openedDirectory + infoVec[row].fileName + '/';
		refresh();
	}
}

void FileSystemModel::upperLevel()
{
	for (int i = openedDirectory.size() - 2; i >= 0; i--)
	{
		if (openedDirectory[i] == '/')
		{
			openedDirectory.resize(i + 1);
			break;
		}
	}
	qDebug() << openedDirectory;
	refresh();
}

static QString getSizeString(quint64 bytes)
{
	auto s = QString::number(bytes);
	if (s.size() > 3)
	{
		double KB = (double)bytes / 1024;
		s = QString::number(KB, 'f', 2);
		if (s.size() > 6)
		{
			double MB = KB / 1024;
			s = QString::number(MB, 'f', 2);
			if (s.size() > 6)
			{
				double GB = MB / 1024;
				s = QString::number(GB, 'f', 2);
				if (s.size() > 6)
				{
					double TB = GB / 1024;
					s = QString::number(TB, 'f', 2);
					return s + " TB";
				}
				return s + " GB";
			}
			return s + " MB";
		}
		return s + " KB";
	}
	return s + " bytes";
}

QVariant FileSystemModel::getDisplayData(int row, int column) const
{
	if (row < 0 || row >= infoVec.size())
	{
		return QVariant();
	}
	switch (column)
	{
	case 0: // 名称
		return infoVec[row].fileName;
	case 1: // 大小
		if (infoVec[row].isDirectory)
		{
			return QVariant();
		}
		return getSizeString(infoVec[row].size);
	case 2: // 类型
		return infoVec[row].isDirectory ? u8"目录" : u8"文件";
	case 3: // 访问权限
		if (infoVec[row].isReadable)
		{
			return infoVec[row].isWritable ? u8"读写" : u8"只读";
		}
		else
		{
			return infoVec[row].isWritable ? u8"只写" : u8"不可访问";
		}
	case 4: // 修改时间
		return infoVec[row].lastModified;
	default:
		return QVariant();
	}
}

QVariant FileSystemModel::getTextAlignment(int column) const
{
	switch (column)
	{
	case 0: // 名称
		return Qt::AlignLeft | Qt::AlignVCenter;
	case 1: // 大小
		return Qt::AlignRight | Qt::AlignVCenter;
	case 2: // 类型
		return Qt::AlignCenter;
	case 3: // 访问权限
		return Qt::AlignCenter;
	case 4: // 修改时间
		return Qt::AlignCenter;
	default:
		return QVariant();
	}
}

LocalFileSystemModel::LocalFileSystemModel(QObject* parent)
	: FileSystemModel(parent)
{
	openedDirectory = QDir::currentPath() + '/';
	fileService.allDirectoriesAccessible = true;
}

void LocalFileSystemModel::refresh()
{
	beginResetModel();
	fileService.listFiles(openedDirectory, infoVec);
	endResetModel();
	emit directoryChanged(openedDirectory);
}

void RemoteFileSystemModel::refresh()
{
	infoVec.clear();
	auto request = NetworkController::instance->newRequest<ListFilesRequest>();
	request->directory = openedDirectory;
	NetworkController::instance->sendRequest(request, 1);
	emit directoryChanged(openedDirectory);
}

void RemoteFileSystemModel::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto listFilesResponse = dynamic_cast<ListFilesResponse*>(response.get()))
	{
		if (response->id > lastMessageId)
		{
			switch (listFilesResponse->result)
			{
			case ListFilesResponse::Result::SUCCESS:
				beginResetModel();
				infoVec = listFilesResponse->infoVec;
				endResetModel();
				return;
			case ListFilesResponse::Result::INVALID_ARGUMENT:
				QMessageBox::critical(nullptr, u8"错误", u8"文件路径无效", QMessageBox::Ok);
				break;
			case ListFilesResponse::Result::NO_SUCH_FILE:
				QMessageBox::critical(nullptr, u8"错误", u8"不存在此目录", QMessageBox::Ok);
				break;
			case ListFilesResponse::Result::CANNOT_ACCESS:
				QMessageBox::critical(nullptr, u8"错误", u8"不可访问该目录", QMessageBox::Ok);
				break;
			}
			openedDirectory.clear();
			refresh();
		}
	}
}
