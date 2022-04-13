#include "stdafx.h"

#include "TransferModel.h"
#include "NetworkController.h"

constexpr int NUM_COLUMNS = 5;

TransferModel::TransferModel(QObject *parent)
	: QAbstractTableModel(parent), uploadWorker(this)
{
}

int TransferModel::rowCount(const QModelIndex& parent) const
{
	return taskList.size();
}

int TransferModel::columnCount(const QModelIndex& parent) const
{
	return NUM_COLUMNS;
}

QVariant TransferModel::data(const QModelIndex& index, int role) const
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

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		static const QString s[NUM_COLUMNS] = { u8"任务类型", u8"任务状态", u8"目标路径", u8"源路径", u8"进度" };
		return section >= 0 && section < NUM_COLUMNS ? s[section] : QVariant();
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}

void TransferModel::addTask(TransferTask::Type type, QString dst, QString src)
{
	beginResetModel();
	taskListMutex.lock();
	TransferTask task;
	task.type = type;
	task.dst = dst;
	task.src = src;
	taskList.push_back(task);
	taskListMutex.unlock();
	endResetModel();

	switch (type)
	{
	case TransferTask::Type::UPLOAD:
		emit newUploadTask();
		break;
	case TransferTask::Type::DOWNLOAD:
		emit newDownloadTask();
		break;
	case TransferTask::Type::LOCAL_COPY:
		emit newLocalCopyTask();
		break;
	case TransferTask::Type::REMOTE_COPY:
		emit newRemoteCopyTask();
		break;
	}
}

QVariant TransferModel::getDisplayData(int row, int column) const
{
	taskListMutex.lock();
	if (row < 0 || row >= taskList.size())
	{
		return QVariant();
	}
	auto iter = taskList.begin();
	for (int i = 0; i < row; i++)
	{
		++iter;
	}
	taskListMutex.unlock();
	switch (column)
	{
	case 0: // 任务类型
		switch (iter->type)
		{
		case TransferTask::Type::UPLOAD:
			return u8"上传";
		case TransferTask::Type::DOWNLOAD:
			return u8"下载";
		case TransferTask::Type::LOCAL_COPY:
			return u8"本地复制";
		case TransferTask::Type::REMOTE_COPY:
			return u8"远程复制";
		}
	case 1: // 任务状态
		switch (iter->state)
		{
		case TransferTask::State::WAIT:
			return u8"等待";
		case TransferTask::State::PREPARE:
			return u8"准备";
		case TransferTask::State::TRANSFER:
			return u8"传输";
		case TransferTask::State::FINISH:
			return u8"完成";
		}
	case 2: // 目标路径
		return iter->dst;
	case 3: // 源路径
		return iter->src;
	case 4: // 进度
		return iter->totalSize == 0 ? QVariant() :
			QString::number(double(iter->transferedSize) / double(iter->totalSize), 'f', 0) + "%";
	default:
		return QVariant();
	}
}

QVariant TransferModel::getTextAlignment(int column) const
{
	switch (column)
	{
	case 0: // 任务类型
		return Qt::AlignCenter;
	case 1: // 任务状态
		return Qt::AlignCenter;
	case 2: // 目标路径
		return Qt::AlignLeft;
	case 3: // 源路径
		return Qt::AlignLeft;
	case 4: // 进度
		return Qt::AlignRight;
	default:
		return QVariant();
	}
}

TransferModel::Worker::Worker(TransferModel* master, TransferTask::Type type)
	: master(master), type(type)
{
	moveToThread(&workerThread);
	workerThread.start();
}

TransferModel::Worker::~Worker()
{
	workerThread.quit();
	workerThread.wait();
}

void TransferModel::Worker::doTask()
{
	master->taskListMutex.lock();
	for (auto iter = master->taskList.begin(); iter != master->taskList.end(); ++iter)
	{
		if (iter->type == type)
		{
			master->taskListMutex.unlock();
			doTaskAt(iter);

			master->beginResetModel();
			master->taskListMutex.lock();
			master->taskList.erase(iter);
			master->taskListMutex.unlock();
			master->endResetModel();
			return;
		}
	}
	master->taskListMutex.unlock();
}

TransferModel::UploadWorker::UploadWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::UPLOAD)
{
	connect(master, &TransferModel::newUploadTask, this, &Worker::doTask);
}

void TransferModel::UploadWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
	master->beginResetModel();
	iter->state = TransferTask::State::PREPARE;
	master->endResetModel();

	QFile file(iter->src);
	file.open(QIODevice::ReadOnly);
	auto request = NetworkController::instance->newRequest<UploadFileRequest>();
	request->dst = iter->dst;
	request->size = file.size();
	NetworkController::instance->sendRequest(request, 1);
	iter->requestId = request->id;
	
	quint64 offset = 0;
	while (!file.atEnd())
	{
		auto request = NetworkController::instance->newRequest<UploadDataRequest>();
		request->offset = offset;
		request->parentId = iter->requestId;
		request->data = file.read(1 << 16); // 64KB
		offset += 1 << 16;
		NetworkController::instance->sendRequest(request, 1);
	}
	file.close();
}

TransferModel::DownloadWorker::DownloadWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::DOWNLOAD)
{
	connect(master, &TransferModel::newDownloadTask, this, &Worker::doTask);
}

void TransferModel::DownloadWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}

TransferModel::LocalCopyWorker::LocalCopyWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::LOCAL_COPY)
{
	connect(master, &TransferModel::newLocalCopyTask, this, &Worker::doTask);
}

void TransferModel::LocalCopyWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}

TransferModel::RemoteCopyWorker::RemoteCopyWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::REMOTE_COPY)
{
	connect(master, &TransferModel::newRemoteCopyTask, this, &Worker::doTask);
}

void TransferModel::RemoteCopyWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}
