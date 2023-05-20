
#ifndef THREADNEWSREPORT_H
#define THREADNEWSREPORT_H


#include <QObject>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QTextToSpeech>
//#include <QRegularExpression>
//#include <QRegularExpressionMatch>

class ThreadNewsReport : public QObject
{
    Q_OBJECT
public:
    explicit ThreadNewsReport(QObject *parent = nullptr);
    void getNewsData();
    void getEastNews();
    QTextToSpeech  *tts;
private:
//    QNetworkAccessManager *naManager;
    QString jinShiNewsReportCurTime;
    int eastNewsReportCurTime;
    QList<QStringList> eastNewsList;
//    bool isInitSpeech=false;
    int count=0;
    QString id;
    bool isRunning=false;
    void initNewsReport(const QByteArray &allData);
    void sayJsNews(QJsonObject object);
    void sayEastNews(QStringList l,int time);
signals:
    void getNewsFinished(QString s);

};

#endif // THREADNEWSREPORT_H
