
#ifndef THREADTIMESHARECHART_H
#define THREADTIMESHARECHART_H

#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QMutex>

class ThreadTimeShareChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTimeShareChart(QObject *parent = nullptr);
    void getAllTimeShareChart();

private:
    void initTimeShareChartList();
    QByteArray allData;
    QMutex m_mutex;

signals:
    void getTimeShareChartFinished();
};

#endif // THREADTIMESHARECHART_H
