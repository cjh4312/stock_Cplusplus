
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    void getAllTimeShareChart();
private:
    QNetworkAccessManager *naManager;
    QByteArray allData;
    bool isRunning=false;
    void initTimeShareChartList();
signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
