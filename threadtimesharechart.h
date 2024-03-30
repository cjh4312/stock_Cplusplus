
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include "globalvar.h"

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    void getSSEData();
    void getAllTimeShareChart(bool r);

private:
    void initTimeShareChartList();
    void initSSETimeShareChartList();
    // QNetworkReply *reply=nullptr;
    // QByteArray* qByteArray=new QByteArray();
    // QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    // QNetworkRequest request;
    QByteArray allData;
    bool reset;
    QString preGCode="";
    int mRetries=0;
    float h;
    float l;
    float pp;

signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
