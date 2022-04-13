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
	case Qt::DisplayRole: // ��Ԫ��������
		return getDisplayData(index.row(), index.column());
	case Qt::TextAlignmentRole: // �����ʽ
		return getTextAlignment(index.column());
	default:
		return QVariant();
	}
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		static const QString s[NUM_COLUMNS] = { u8"����", u8"��С", u8"����", u8"����Ȩ��", u8"�޸�ʱ��"};
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
	return openedDirectory + "/" + fileName;
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
	case 0: // ����
		return infoVec[row].fileName;
	case 1: // ��С
		if (infoVec[row].isDirectory)
		{
			return QVariant();
		}
		return getSizeString(infoVec[row].size);
	case 2: // ����
		return infoVec[row].isDirectory ? u8"Ŀ¼" : u8"�ļ�";
	case 3: // ����Ȩ��
		if (infoVec[row].isReadable)
		{
			return infoVec[row].isWritable ? u8"��д" : u8"ֻ��";
		}
		else
		{
			return infoVec[row].isWritable ? u8"ֻд" : u8"���ɷ���";
		}
	case 4: // �޸�ʱ��
		return infoVec[row].lastModified;
	default:
		return QVariant();
	}
}

QVariant FileSystemModel::getTextAlignment(int column) const
{
	switch (column)
	{
	case 0: // ����
		return Qt::AlignLeft;
	case 1: // ��С
		return Qt::AlignRight;
	case 2: // ����
		return Qt::AlignCenter;
	case 3: // ����Ȩ��
		return Qt::AlignCenter;
	case 4: // �޸�ʱ��
		return Qt::AlignCenter;
	default:
		return QVariant();
	}
}

LocalFileSystemModel::LocalFileSystemModel(QObject* parent)
	: FileSystemModel(parent)
{
	fileService.allDirectoriesAccessible = true;
	refresh();
}

void LocalFileSystemModel::refresh()
{
	openedDirectory = "D:/test";
	beginResetModel();
	fileService.listFiles(openedDirectory, infoVec);
	endResetModel();
}

void RemoteFileSystemModel::refresh()
{
	openedDirectory = "D:/test";
	auto request = NetworkController::instance->newRequest<ListFilesRequest>();
	request->directory = openedDirectory;
	NetworkController::instance->sendRequest(request, 1);
}

void RemoteFileSystemModel::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto listFilesResponse = dynamic_cast<ListFilesResponse*>(response.get()))
	{
		if (response->id > lastMessageId)
		{
			beginResetModel();
			infoVec = listFilesResponse->infoVec;
			endResetModel();
		}
	}
}
