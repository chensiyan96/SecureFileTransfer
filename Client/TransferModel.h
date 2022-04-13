#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"
#include "../SecureFileTransfer/FileService.h"

struct TransferTask
{
	enum class Type
	{
		UPLOAD, DOWNLOAD, LOCAL_COPY, REMOTE_COPY
	} type;
	enum class State
	{
		WAIT, PREPARE, TRANSFER, FINISH
	} state = State::WAIT;
	QString dst;
	QString src;
	quint64 transferedSize = 0;
	quint64 totalSize = 0;
};

class TransferModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit TransferModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void addTask(TransferTask::Type type, QString dst, QString src);

signals:
	void newUploadTask();
	void newDownloadTask();
	void newLocalCopyTask();
	void newRemoteCopyTask();

private:
	QVariant getDisplayData(int row, int column) const;
	QVariant getTextAlignment(int column) const;

	struct Worker : public QObject
	{
		Worker(TransferModel* master, TransferTask::Type type);
		~Worker();
		void doTask();
		virtual void doTaskAt(QLinkedList<TransferTask>::iterator iter) = 0;

		QThread workerThread;
		TransferModel* master;
		TransferTask::Type type;
	};

	struct UploadWorker : public Worker
	{
		UploadWorker(TransferModel* master);
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

	struct DownloadWorker : public Worker
	{
		DownloadWorker(TransferModel* master);
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

	struct LocalCopyWorker : public Worker
	{
		LocalCopyWorker(TransferModel* master);
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

	struct RemoteCopyWorker : public Worker
	{
		RemoteCopyWorker(TransferModel* master);
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

private:
	QLinkedList<TransferTask> taskList;
	mutable QMutex taskListMutex;
	UploadWorker uploadWorker;
};
