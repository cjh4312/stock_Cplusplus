
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
private:
    QNetworkAccessManager *naManager;
    QByteArray allData;
    bool isRunning=false;
    void initIndexList();
    void initFuturesList();

signals:
    void getIndexFinished();
};

#endif // THREADINDEX_H
