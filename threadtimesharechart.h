
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    void getSSEData();
    void getAllTimeShareChart();

private:
    void initTimeShareChartList(QByteArray allData);
    void initTimeShareChartList1(QByteArray allData);

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
