
#ifndef THREADNEWSREPORT_H
#define THREADNEWSREPORT_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QTextToSpeech>

class ThreadNewsReport : public QObject
{
    Q_OBJECT
public:
    explicit ThreadNewsReport(QObject *parent = nullptr);
    void getNewsData();
private:
    QNetworkAccessManager *naManager;
    QByteArray allData;
    QTextToSpeech  *tts;
    QString jinShiNewsReportCurTime;
    bool isInitSpeech=false;
    int count=0;
    QString id;
    bool isRunning=false;
    void initNewsReport();
signals:
    void getNewsFinished(QString s);

};

#endif // THREADNEWSREPORT_H
