
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
private:
//    QNetworkAccessManager *naManager;
    bool isRunning=false;
    void initIndexList(const QByteArray &allData);
    void initFuturesList(const QByteArray &allData);

signals:
    void getIndexFinished();
};

#endif // THREADINDEX_H
