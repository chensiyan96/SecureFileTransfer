#include "stdafx.h"

#include "ServerWidget.h"
#include "StateController.h"
#include "RequestController.h"

void outputMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("debug:");
        break;
    case QtInfoMsg:
        text = QString("info:");
        break;
    case QtWarningMsg:
        text = QString("warning:");
        break;
    case QtCriticalMsg:
        text = QString("critical:");
        break;
    case QtFatalMsg:
        text = QString("fatal:");
        break;
    }

    QString current_date_time = QDateTime::currentDateTime().toString();
    QString message = QString("%1 %2 %3").arg(text).arg(msg).arg(current_date_time);

    static QMutex mutex;
    mutex.lock();
    ServerWidget::instance->outputLog(message);
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<Request>>("QSharedPointer<Request>");
    qRegisterMetaType<QSharedPointer<Response>>("QSharedPointer<Response>");

    QApplication a(argc, argv);
    ServerWidget::instance = new ServerWidget;
    ServerWidget::instance->show();
    qInstallMessageHandler(outputMessage);
    StateController::instance = new StateController;

    auto ret = a.exec();
    delete StateController::instance;
    return ret;
}
