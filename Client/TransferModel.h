#pragma once

#include "../SecureFileTransfer/AppLayerMessage.h"
#include "../SecureFileTransfer/FileService.h"

#include <map>

struct TransferTask
{
	enum class Type
	{
		UPLOAD, DOWNLOAD, LOCAL_COPY, REMOTE_COPY
	} type;
	enum class State
	{
		WAIT, PREPARE, TRANSFER, FAILED
	} state = State::WAIT;
	QString dst;
	QString src;
	quint64 transferedSize = 0;
	quint64 totalSize = 0;
	quint32 requestId = 0;
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

	void onMainStateEntry();
	void onMainStateExit();
	void addTask(TransferTask::Type type, QString dst, QString src);

signals:
	void newUploadTask();
	void newDownloadTask();
	void newLocalCopyTask();
	void newRemoteCopyTask();

	void taskFinished();

private:
	QVariant getDisplayData(int row, int column) const;
	QVariant getTextAlignment(int column) const;

	struct Worker : public QObject
	{
		Worker(TransferModel* master, TransferTask::Type type);
		~Worker();
		void doTask();
		virtual void prepareTaskAt(QLinkedList<TransferTask>::iterator iter) = 0;
		virtual void doTaskAt(QLinkedList<TransferTask>::iterator iter) = 0;

		QThread workerThread;
		TransferModel* master;
		TransferTask::Type type;
		QQueue<QLinkedList<TransferTask>::iterator> preparedQueue;
		int numRequests = 0;
	};

	struct RequestRecord
	{
		std::unique_ptr<QFile> file;
		QSet<quint32> idSet;
		QLinkedList<TransferTask>::iterator iter;
		quint64 offset = 0;
	};

	struct UploadWorker : public Worker
	{
		UploadWorker(TransferModel* master);
		void prepareTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);
		std::map<quint32, RequestRecord> requestMap;
	};

	struct DownloadWorker : public Worker
	{
		DownloadWorker(TransferModel* master);
		void prepareTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void checkResponse(QSharedPointer<Request> request, QSharedPointer<Response> response);
		std::map<quint32, RequestRecord> requestMap;
	};

	struct LocalCopyWorker : public Worker
	{
		LocalCopyWorker(TransferModel* master);
		void prepareTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

	struct RemoteCopyWorker : public Worker
	{
		RemoteCopyWorker(TransferModel* master);
		void prepareTaskAt(QLinkedList<TransferTask>::iterator iter) override;
		void doTaskAt(QLinkedList<TransferTask>::iterator iter) override;
	};

private:
	QLinkedList<TransferTask> taskList;
	mutable QMutex taskListMutex;
	UploadWorker uploadWorker;
};
