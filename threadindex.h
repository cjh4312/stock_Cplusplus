
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include <QObject>
#include <QJsonArray>

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
private:
    void initIndexList();
    void initFuturesList();
    QByteArray indexData;
    QByteArray futuresData;

signals:
    void getIndexFinished();
};

#endif // THREADINDEX_H
