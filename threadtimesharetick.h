
#ifndef THREADTIMESHARETICK_H
#define THREADTIMESHARETICK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadTimeShareTick : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareTick(QObject *parent = nullptr);
    void getAllBuySell();
    void getAllTimeShareTick();
private:
    QNetworkAccessManager *naManager;
    QByteArray allData;
    bool isRunning=false;
    void initBuySellList();
    void initTimeShareTickList();
signals:
    void getTimeShareTickFinished();
    void getBuySellFinished();
};

#endif // THREADTIMESHARETICK_H
