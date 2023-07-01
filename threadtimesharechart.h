
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
    void getAllTimeShareChart();
private:
    void initTimeShareChartList(const QByteArray &allData);
signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
