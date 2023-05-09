
#ifndef THREADTIMESHARETICK_H
#define THREADTIMESHARETICK_H

#include <QObject>
#include <QNetworkAccessManager>
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
    QNetworkAccessManager *naManager;
    QByteArray allData;
    bool isRunning=false;
    void initBuySellList();
    void initTimeShareTickList();
    void findStockArea();
signals:
    void getTimeShareTickFinished();
    void getBuySellFinished();
};

#endif // THREADTIMESHARETICK_H
