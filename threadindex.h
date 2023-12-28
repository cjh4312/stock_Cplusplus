
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include "globalvar.h"

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
private:
    void initIndexList();
    void getEastFundFlow();
    QByteArray indexData;
//    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);

signals:
    void getIndexFinished();
};

#endif // THREADINDEX_H
