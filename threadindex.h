
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>
//#include <QMutex>

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
private:
    void initIndexList(QByteArray indexData);
    void initFuturesList(QByteArray futuresData);

//    QMutex m_mutex;

signals:
    void getIndexFinished();
};

#endif // THREADINDEX_H
