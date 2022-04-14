#include "stdafx.h"

#include "TransferModel.h"
#include "NetworkController.h"

constexpr int NUM_COLUMNS = 5;
constexpr int MAX_CONCURRENT_REQUESTS = 1024;

TransferModel::TransferModel(QObject* parent)
	: QAbstractTableModel(parent), uploadWorker(this), downloadWorker(this)
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

void TransferModel::onMainStateEntry()
{
	connect(NetworkController::instance, &NetworkController::receivedResponse, &uploadWorker, &UploadWorker::checkResponse);
	connect(NetworkController::instance, &NetworkController::receivedResponse, &downloadWorker, &DownloadWorker::checkResponse);

}

void TransferModel::onMainStateExit()
{
	NetworkController::instance->disconnect(&uploadWorker);
	NetworkController::instance->disconnect(&downloadWorker);

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
		case TransferTask::State::FAILED:
			return u8"失败";
		}
	case 2: // 目标路径
		return iter->dst;
	case 3: // 源路径
		return iter->src;
	case 4: // 进度
		return iter->totalSize == 0 ? QVariant() :
			QString::number(double(iter->transferedSize) / double(iter->totalSize) * 100.0, 'f', 0) + "%";
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
	master->beginResetModel();
	master->taskListMutex.lock();
	for (auto iter = master->taskList.begin(); iter != master->taskList.end(); ++iter)
	{
		if (iter->type == type && iter->state == TransferTask::State::WAIT)
		{
			prepareTaskAt(iter);
		}
	}
	master->taskListMutex.unlock();
	master->endResetModel();
	for (auto iter : preparedQueue)
	{
		if (numRequests >= MAX_CONCURRENT_REQUESTS)
		{
			// 不能同时发送过多数据，会堵塞在socket中
			return;
		}
		if (iter->state == TransferTask::State::TRANSFER)
		{
			doTaskAt(iter);
		}
	}
}

TransferModel::UploadWorker::UploadWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::UPLOAD)
{
	connect(master, &TransferModel::newUploadTask, this, &Worker::doTask);
}

void TransferModel::UploadWorker::prepareTaskAt(QLinkedList<TransferTask>::iterator iter)
{
	RequestRecord record = { std::make_unique<QFile>(iter->src), QSet<quint32>(), iter };
	record.file->open(QIODevice::ReadOnly);
	if (!record.file->isOpen())
	{
		iter->state = TransferTask::State::FAILED;
		return;
	}
	auto request = NetworkController::instance->newRequest<UploadFileRequest>();
	request->dst = iter->dst;
	request->size = record.file->size();
	NetworkController::instance->sendRequest(request, 1);
	iter->requestId = request->id;
	iter->state = TransferTask::State::PREPARE;
	iter->totalSize = record.file->size();
	requestMap.emplace(request->id, std::move(record));
	preparedQueue.push_back(iter);
}

void TransferModel::UploadWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
	auto& record = requestMap[iter->requestId];
	assert(record.file != nullptr);
	if (!record.file->isOpen())
	{
		// 文件关闭表示已经发送完成
		return;
	}
	record.file->seek(record.offset);
	while (!record.file->atEnd())
	{
		auto request = NetworkController::instance->newRequest<UploadDataRequest>();
		request->offset = record.offset;
		request->parentId = iter->requestId;	
		request->data = record.file->read(1u << 16); // 64KB
		record.offset += request->data.size();
		NetworkController::instance->sendRequest(request, 2);
		record.idSet.insert(request->id);
		if (++numRequests >= MAX_CONCURRENT_REQUESTS)
		{
			// 不能同时发送过多数据，会堵塞在socket中
			return;
		}
	}
	record.file->close();
	preparedQueue.pop_front();
}

void TransferModel::UploadWorker::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto uploadDataResponse = dynamic_cast<UploadFileResponse*>(response.get()))
	{
		auto uploadDataRequest = dynamic_cast<UploadFileRequest*>(request.get());
		auto& record = requestMap[uploadDataRequest->id];
		assert(record.file != nullptr);
		master->beginResetModel();
		record.iter->state = TransferTask::State::TRANSFER;
		master->endResetModel();
		doTask();
	}

	if (auto uploadDataResponse = dynamic_cast<UploadDataResponse*>(response.get()))
	{
		auto uploadDataRequest = dynamic_cast<UploadDataRequest*>(request.get());
		auto& record = requestMap[uploadDataRequest->parentId];
		assert(record.file != nullptr);
		record.idSet.remove(uploadDataResponse->id);
		--numRequests;
		qDebug() << record.idSet.size();

		master->beginResetModel();
		record.iter->transferedSize += uploadDataRequest->data.size();
		if (!record.file->isOpen() && record.idSet.isEmpty())
		{
			master->taskListMutex.lock();
			master->taskList.erase(record.iter);
			requestMap.erase(uploadDataRequest->parentId);
			master->taskListMutex.unlock();
			emit master->taskFinished();
		}
		master->endResetModel();
		doTask();
	}
}

TransferModel::DownloadWorker::DownloadWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::DOWNLOAD)
{
	connect(master, &TransferModel::newDownloadTask, this, &Worker::doTask);
}

void TransferModel::DownloadWorker::prepareTaskAt(QLinkedList<TransferTask>::iterator iter)
{
	RequestRecord record = { std::make_unique<QFile>(iter->dst), QSet<quint32>(), iter };
	record.file->open(QIODevice::WriteOnly);
	if (!record.file->isOpen())
	{
		iter->state = TransferTask::State::FAILED;
		return;
	}
	auto request = NetworkController::instance->newRequest<DownloadFileRequest>();
	request->src = iter->src;
	NetworkController::instance->sendRequest(request, 1);
	iter->requestId = request->id;
	iter->state = TransferTask::State::PREPARE;
	requestMap.emplace(request->id, std::move(record));
	preparedQueue.push_back(iter);
}

void TransferModel::DownloadWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
	auto& record = requestMap[iter->requestId];
	assert(record.file != nullptr);
	while (record.offset < record.iter->totalSize)
	{
		auto request = NetworkController::instance->newRequest<DownloadDataRequest>();
		request->offset = record.offset;
		request->parentId = iter->requestId;
		request->size = std::min(1u << 16, quint32(record.iter->totalSize - record.offset)); // 64KB
		record.offset += request->size;
		NetworkController::instance->sendRequest(request, 1);
		record.idSet.insert(request->id);
		if (++numRequests >= MAX_CONCURRENT_REQUESTS)
		{
			// 不能同时发送过多数据，会堵塞在socket中
			return;
		}
	}
	preparedQueue.pop_front();
}

void TransferModel::DownloadWorker::checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response)
{
	if (auto downloadDataResponse = dynamic_cast<DownloadFileResponse*>(response.get()))
	{
		auto downloadDataRequest = dynamic_cast<DownloadFileRequest*>(request.get());
		auto& record = requestMap[downloadDataRequest->id];
		assert(record.file != nullptr);
		master->beginResetModel();
		record.iter->state = TransferTask::State::TRANSFER;
		record.iter->totalSize = downloadDataResponse->size;
		master->endResetModel();
		doTask();
	}

	if (auto downloadDataResponse = dynamic_cast<DownloadDataResponse*>(response.get()))
	{
		auto downloadDataRequest = dynamic_cast<DownloadDataRequest*>(request.get());
		auto& record = requestMap[downloadDataRequest->parentId];
		assert(record.file != nullptr);
		record.idSet.remove(downloadDataResponse->id);
		--numRequests;
		record.file->seek(downloadDataRequest->offset);
		record.file->write(downloadDataResponse->data);

		qDebug() << record.idSet.size();
		master->beginResetModel();
		record.iter->transferedSize += downloadDataResponse->data.size();
		if (record.iter->transferedSize >= record.iter->totalSize)
		{
			assert(record.iter->transferedSize == record.iter->totalSize);
			master->taskListMutex.lock();
			master->taskList.erase(record.iter);
			requestMap.erase(downloadDataRequest->parentId);
			master->taskListMutex.unlock();
			emit master->taskFinished();
		}
		master->endResetModel();
		doTask();
	}
}

TransferModel::LocalCopyWorker::LocalCopyWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::LOCAL_COPY)
{
	connect(master, &TransferModel::newLocalCopyTask, this, &Worker::doTask);
}

void TransferModel::LocalCopyWorker::prepareTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}

void TransferModel::LocalCopyWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}

TransferModel::RemoteCopyWorker::RemoteCopyWorker(TransferModel* master)
	: Worker(master, TransferTask::Type::REMOTE_COPY)
{
	connect(master, &TransferModel::newRemoteCopyTask, this, &Worker::doTask);
}

void TransferModel::RemoteCopyWorker::prepareTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}

void TransferModel::RemoteCopyWorker::doTaskAt(QLinkedList<TransferTask>::iterator iter)
{
}
