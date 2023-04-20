
#include "threadnewsreport.h"
#include "globalvar.h"

ThreadNewsReport::ThreadNewsReport(QObject *parent)
    : QObject{parent}
{
    naManager = new QNetworkAccessManager(this);
}

void ThreadNewsReport::getNewsData()
{
    if (isRunning)
        return;
    isRunning=true;
    GlobalVar::getEastData(naManager,allData,1.3,QUrl("https://www.jin10.com/flash_newest.js?t=1667528593473"));
    initNewsReport();
    GlobalVar::isNewsReport=false;
    isRunning=false;
}

void ThreadNewsReport::initNewsReport()
{
    if (not isInitSpeech)
    {
        isInitSpeech=true;
        tts = new QTextToSpeech(this);
        tts->setLocale(QLocale::Chinese);//设置语言环境
        tts->setRate(0.2);//设置语速-1.0到1.0
        tts->setPitch(0.0);//设置音高-1.0到1.0
        tts->setVolume(0.6);//设置音量0.0-1.0
        id=QDateTime::currentDateTime().addSecs(-1800).toString("yyyyMMddhhmmss");
        jinShiNewsReportCurTime = GlobalVar::settings->value("jinShiNewsReportCurTime").toString();
//        qDebug()<<jinShiNewsReportCurTime;
    }
    QString cur_time=QDateTime::currentDateTime().toString("yyyyMMddhhmmss").mid(10,2);
    if (cur_time <= "01")
    {
        if (count < 3)
        {
            if (tts->state() == QTextToSpeech::Ready)
                tts->say("休息时间,起来锻炼了");
            count+=1;
        }
    }
    else if (("20" <= cur_time and cur_time <= "21") or ("40" <= cur_time and cur_time<= "41"))
    {
        if (count < 3)
        {
            if (tts->state() == QTextToSpeech::Ready)
                tts->say("转转头,伸个懒腰");
            count+=1;
        }
    }
    else
        count = 0;
    allData=allData.mid(13,allData.size()-14);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QString newId;
        QString newsText;
        QJsonArray array = doc.array();
        QString dt;
        for (int i = array.size()-1; i >-1 ; --i)
        {
            newId=array[i].toObject().value("id").toString().mid(0,14);
//            qDebug()<<id<<newId;
            if (id>=newId)
                continue;
            if (array[i].toObject().value("data").toObject().contains("content"))
            {
                if (tts->state() == QTextToSpeech::Ready)
                {
                    newsText=array[i].toObject().value("data").toObject().value("content").toString();
                    if (newsText.contains("<a") or newsText.contains("点击查看") or newsText.contains("金十图示") or
                        newsText.contains("＞＞") or newsText.contains("...") or newsText.contains("......") or
                        newsText.contains(">>") or newsText.contains("……"))
                        continue;
                    if (newsText=="" or newsText=="-")
                        continue;
                    dt=QDateTime::fromString(array[i].toObject().value("time").toString().mid(0,19), "yyyy-MM-ddThh:mm:ss").addSecs(28800).toString("yyyy-MM-dd hh:mm:ss");
                    if (jinShiNewsReportCurTime>=dt)
                        continue;
                    if (GlobalVar::isSayNews)
                        tts->say(newsText);
                    id=newId;
                    emit getNewsFinished("<font size=\"4\" color=red>"+dt+"</font>"+"<font size=\"4\">"+newsText+"</font>");
                    GlobalVar::settings->setValue("jinShiNewsReportCurTime",dt);
                }
            }
        }
    }
}

