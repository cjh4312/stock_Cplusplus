#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include "qobjectdefs.h"
#include <QRunnable>
#include <QString>
#include "globalvar.h"
#include "QFile"
#include "QMutex"

class DownloadTask : public QRunnable
{
public:
    explicit DownloadTask(QObject *obj);
    static int nums;
    QString curDate;
    QString endDate;
    QMutex mutex;
    void run() override
    {
        downloadK();
        QMetaObject::invokeMethod(m_pObj,"setText",Q_ARG(int,nums));
    }
    void downloadK();
private:
    QObject * m_pObj = nullptr;
};



#endif // DOWNLOADTASK_H
