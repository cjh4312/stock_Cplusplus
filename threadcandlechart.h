
#ifndef THREADCANDLECHART_H
#define THREADCANDLECHART_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadCandleChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadCandleChart(QObject *parent = nullptr);
    void getAllCandleChart(QString freq,QString adjustFlag);
private:
    QNetworkAccessManager *naManager;
    QByteArray allData;
    bool isRunning=false;
    void initCandleChartList();
signals:
    void getCandleChartFinished();
};

#endif // THREADCANDLECHART_H
