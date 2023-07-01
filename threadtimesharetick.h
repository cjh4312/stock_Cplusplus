
#ifndef THREADTIMESHARETICK_H
#define THREADTIMESHARETICK_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QFile>
#include <QTextCodec>

class ThreadTimeShareTick : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareTick(QObject *parent = nullptr);
    void getBuySellTimeShareTick();

private:
    void initBuySellList(const QByteArray &allData);
    void initTimeShareTickList(const QByteArray &allData);
    void findStockArea();
    QList<QStringList> areaData;
signals:
    void getTimeShareTickFinished();
    void getBuySellFinished();
};

#endif // THREADTIMESHARETICK_H
