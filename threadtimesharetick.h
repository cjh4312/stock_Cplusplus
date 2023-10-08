
#ifndef THREADTIMESHARETICK_H
#define THREADTIMESHARETICK_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QFile>
#include <QTextCodec>
#include <QMutex>

class ThreadTimeShareTick : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareTick(QObject *parent = nullptr);
    void getBuySellTimeShareTick();

private:
    void initBuySellList(QByteArray buySellData);
    void initTimeShareTickList(QByteArray timeShareTickData);
    void findStockArea();

    QList<QStringList> areaData;
    QString price[10]={"f31","f33","f35","f37","f39","f19","f17","f15","f13","f11"};
    QString nums[10]={"f32","f34","f36","f38","f40","f20","f18","f16","f14","f12"};
    QString baseInfo[14]={"f43","f170","f168","f48","f84","f116","f167","f46","f44","f45","f47","f85","f108","f162"};

    QMutex m_mutex;

signals:
    void getTimeShareTickFinished();
    void getBuySellFinished();
};

#endif // THREADTIMESHARETICK_H
