
#ifndef THREADNEWSREPORT_H
#define THREADNEWSREPORT_H


#include <QObject>
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
    QTextToSpeech  *tts;

private:
    QByteArray allData;
    QString jinShiNewsReportCurTime;
    int eastNewsReportCurTime;
    QList<QStringList> eastNewsList;
    int count=0;
    QString id;
    void initEastNews();
    void initNewsReport();
    void sayJsNews(QJsonObject object);
    void sayEastNews(QStringList l,int time);

signals:
    void getNewsFinished(QString s);

};

#endif // THREADNEWSREPORT_H
