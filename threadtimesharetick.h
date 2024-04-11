
#ifndef THREADTIMESHARETICK_H
#define THREADTIMESHARETICK_H

#include "globalvar.h"
#include <QFile>
#include <QTextCodec>

class ThreadTimeShareTick : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareTick(QObject *parent = nullptr);
    void getBuySellTimeShareTick(bool reset);
    void getSSEData(int nums,QString url);

private:
    void initBuySellList();
    void initTimeShareTickList(QString pos);
    void initSSETimeShareTickList();
    void findStockArea();
    QString preCode="";
    QByteArray buySellData;
    QByteArray timeShareTickData;
    // int mRetries=0;

    QList<QStringList> areaData;
    QString price[10]={"f31","f33","f35","f37","f39","f19","f17","f15","f13","f11"};
    QString nums[10]={"f32","f34","f36","f38","f40","f20","f18","f16","f14","f12"};
    QString baseInfo[14]={"f43","f170","f168","f48","f84","f116","f167","f46","f44","f45","f47","f85","f55","f162"};

signals:
    void getTimeShareTickFinished();
    void getBuySellFinished();
};

#endif // THREADTIMESHARETICK_H
