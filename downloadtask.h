#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include "qobjectdefs.h"
#include <QRunnable>
#include <QString>
#include "globalvar.h"
#include "QFile"

class DownloadTask : public QRunnable
{
public:
    explicit DownloadTask(QObject *obj);
    int nums;
    QString curDate;
    QString endDate;
    void run() override
    {
        downloadK();
        QMetaObject::invokeMethod(m_pObj,"setText");
    }
    void downloadK();
private:
    QObject * m_pObj = nullptr;
};



#endif // DOWNLOADTASK_H
