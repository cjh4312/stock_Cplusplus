
#include "threadnewsreport.h"

ThreadNewsReport::ThreadNewsReport(QObject *parent)
    : QObject{parent}
{
    tts->setLocale(QLocale::Chinese);
    tts->setRate(0.3);
    tts->setPitch(0.0);
    tts->setVolume(1);
    QDateTime c=QDateTime::currentDateTime();
    id=c.addSecs(-1800).toString("yyyyMMddhhmmss");
    jinShiNewsReportCurTime = GlobalVar::settings->value("jinShiNewsReportCurTime").toString();
    eastNewsReportCurTime = GlobalVar::settings->value("eastNewsReportCurTime").toInt();
    if (eastNewsReportCurTime>12000000 and c.date().month()==1)
        eastNewsReportCurTime=1010000;
}

void ThreadNewsReport::getNewsData()
{
    if (tts->state() == QTextToSpeech::Speaking)
        return;
    newsContent="";
    GlobalVar::getData(allData,2,QUrl("https://finance.eastmoney.com/yaowen.html"));
    if (GlobalVar::timeOutFlag[4])
        GlobalVar::timeOutFlag[4]=false;
    else
        initEastNews();
    GlobalVar::getData(allData,2,QUrl("https://www.jin10.com/flash_newest.js?t=1667528593473"));

    if (GlobalVar::timeOutFlag[3])
        GlobalVar::timeOutFlag[3]=false;
    else
        initNewsReport();
}

void ThreadNewsReport::initEastNews()
{
    QString s=GlobalVar::peelStr(QString(allData),"id=\"artitileList1\"","-1");
    QPair<QString, QString> pair;
    eastNewsList.clear();
    for (int i=1;i<=50;++i)
    {
        QStringList dataList;
        pair=GlobalVar::cutStr(s,"<li id","</li>");
        QString content=GlobalVar::peelStr(pair.first,"<a ","-1");
        s=pair.second;
        QString temp="href=\"";
        dataList<<content.mid(content.indexOf(temp)+temp.length(),GlobalVar::peelStr(content,temp,"-1").indexOf("\""));
        dataList<<GlobalVar::getContent(content);
        dataList<<GlobalVar::getContent(GlobalVar::peelStr(content,"class=\"time\"","-1"));
        // qDebug()<<dataList;
        eastNewsList.append(dataList);
    }
    std::sort(eastNewsList.begin(),eastNewsList.end(),[](QStringList a,QStringList b){
        return a[2]>b[2];
    });
}

void ThreadNewsReport::initNewsReport()
{
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
    QByteArray temp=allData.mid(13,allData.size()-14);
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(temp, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonArray array = doc.array();
        int eastNums=eastNewsList.count()-1;
        int jsNums=array.size()-1;
        while(eastNums!=-1 or jsNums!=-1)
        {
            QString eastTime;
            if (eastNums!=-1)
            {
                QString str=eastNewsList.at(eastNums)[2];
                int a=str.indexOf("月");
                QString m=str.mid(0,a);
                QString d=str.mid(a+1,str.indexOf("日")-a-1);
                if (d.length()==1)
                    d="0"+d;
                QString s=str.right(5);
                eastTime=m+d+s.left(2)+s.right(2);
//                qDebug()<<eastTime;
            }
            else
                eastTime="123456789";
            QString jsTime;

            if (jsNums!=-1)
                jsTime=array[jsNums].toObject().value("id").toString().mid(4,8);
            else
                jsTime="123456789";

            int et=eastTime.toInt();
            // qDebug()<<jsTime.toInt()<<et;

            if (jsTime.toInt()>et and eastNums!=-1)
            {
                sayEastNews(eastNewsList[eastNums],et);
                eastNums-=1;
            }
            else if (jsNums!=-1)
            {
                sayJsNews(array[jsNums].toObject());
                jsNums-=1;
            }
        }
        tts->say(newsContent);
    }
}

void ThreadNewsReport::sayJsNews(QJsonObject object)
{
    QString newId=object.value("id").toString().mid(0,14);
    if (id>=newId)
        return;
    if (object.value("data").toObject().contains("content"))
    {
        QString newsText=object.value("data").toObject().value("content").toString();
        if (newsText.contains("<a") or newsText.contains("点击查看") or newsText.contains("金十图示") or
            newsText.contains("＞＞") or newsText.contains("...") or newsText.contains("......") or
            newsText.contains(">>") or newsText.contains("……") or newsText.contains("免费领取")
            or newsText.contains("盯盘神器"))
            return;
        if (newsText=="" or newsText=="-")
            return;
        // QString dt=QDateTime::fromString(object.value("time").toString().mid(0,19), "yyyy-MM-ddThh:mm:ss").addSecs(28800).toString("yyyy-MM-dd hh:mm:ss");
        QString dt=object.value("time").toString();
        if (jinShiNewsReportCurTime>=dt)
            return;
        newsContent+=newsText;
        // if (GlobalVar::isSayNews)
        //     tts->say(newsText);
        id=newId;
        emit getNewsFinished("<font size=\"4\" color=red>"+dt+"</font>"+"<font size=\"4\">"+
                             newsText+"</font>");
        GlobalVar::settings->setValue("jinShiNewsReportCurTime",dt);
    }
}

void ThreadNewsReport::sayEastNews(QStringList l, int time)
{
    if (eastNewsReportCurTime>=time)
        return;
    QString newsText=l[1];
    newsContent+="东方财经:"+newsText+"。";
    // if (GlobalVar::isSayNews)
    //     tts->say("东方财经:"+newsText);
    emit getNewsFinished("<font size=\"4\" color=red>"+l[2]+"</font>"+"<span> <a href="+l[0]+">"+
                         "<font size=\"4\">"+newsText+"</font>"+"</a> </span>");
    eastNewsReportCurTime=time;
    GlobalVar::settings->setValue("eastNewsReportCurTime",time);
}

