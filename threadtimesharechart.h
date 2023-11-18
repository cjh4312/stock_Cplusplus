
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include "qnetworkreply.h"
#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    void getSSEData();
    void getAllTimeShareChart(bool reset);

private:
    void initTimeShareChartList(QByteArray allData);
    void initTimeShareChartList1(QByteArray allData);
    QNetworkReply *reply=nullptr;

    QString preGCode="";
    int mRetries=0;
    bool isFirst=true;
    float h;
    float l;
    float pp;

signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
