
#ifndef THREADCANDLECHART_H
#define THREADCANDLECHART_H


#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>

class ThreadCandleChart : public QObject
{
    Q_OBJECT
public:
    explicit ThreadCandleChart(QObject *parent = nullptr);
    void getAllCandleChart(QString freq,QString adjustFlag,bool isFirst);
private:
    void initCandleChartList(const QByteArray &allData);
signals:
    void getCandleChartFinished();
};

#endif // THREADCANDLECHART_H
