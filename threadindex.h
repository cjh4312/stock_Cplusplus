
#ifndef THREADINDEX_H
#define THREADINDEX_H

#include "globalvar.h"

class ThreadIndex : public QObject
{
    Q_OBJECT
public:
    explicit ThreadIndex(QObject *parent = nullptr);
    void getAllIndex();
    double totalAmount;
private:
    void initIndexList();
    void getEastFundFlow();
    bool isFirst=true;
    QByteArray indexData;

signals:
    void getIndexFinished();
    void getBlockFinished();
};

#endif // THREADINDEX_H
