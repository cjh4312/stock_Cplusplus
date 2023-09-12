
#include "threadtimesharechart.h"
#include "globalvar.h"

ThreadTimeShareChart::ThreadTimeShareChart(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
}

void ThreadTimeShareChart::getAllTimeShareChart()
{

    GlobalVar::getData(allData,1.5,QUrl("https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893"));
    if (GlobalVar::timeOutFlag[6])
        GlobalVar::timeOutFlag[6]=false;
    else
        {
            initTimeShareChartList();
            emit getTimeShareChartFinished();
        }
}

void ThreadTimeShareChart::initTimeShareChartList()
{
    m_mutex.lock();

    QJsonParseError *jsonError=new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, jsonError);
    if (jsonError->error == QJsonParseError::NoError)
    {
//        QList<timeShartChartInfo> timeShareChartList;
        QJsonObject jsonObject = doc.object();
        GlobalVar::preClose=jsonObject.value("data").toObject().value("preClose").toDouble();
        int ph=110;
        int pl=90;
        if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(3)=="688")
        {
            ph=120;
            pl=80;
        }
        float a=int(GlobalVar::preClose*ph+0.5)/100.0;
        float b=int(GlobalVar::preClose*pl+0.5)/100.0;

        GlobalVar::timeShareHighLowPoint[3]=per(a);
        GlobalVar::timeShareHighLowPoint[4]=per(b);

        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
        float h=0.0;
        float l=100000.0;
        GlobalVar::timeShareHighLowPoint[2]=0;
        float pp=GlobalVar::preClose;
        GlobalVar::mTimeShareChartList.clear();
        if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
            for (int i = 0; i < data.size(); ++i)
            {
                timeShartChartInfo info;
                QStringList list=data.at(i).toString().split(",");
                if (pp<list[2].toFloat())
                    info.direct=2;
                else if (pp>list[2].toFloat())
                    info.direct=1;
                else
                    info.direct=3;
                pp=list[2].toFloat();
                info.time=list[0];
                info.price=per(pp);
                info.vol=list[5].toFloat();
                info.avePrice=per(list[7].toFloat());
                if (list[3].toFloat()>h)
                    h=list[3].toFloat();
                if (list[4].toFloat()<l)
                    l=list[4].toFloat();
                if (list[7].toFloat()>h)
                    h=list[7].toFloat();
                if (list[7].toFloat()<l)
                    l=list[7].toFloat();

                if (list[5].toFloat()>GlobalVar::timeShareHighLowPoint[2])
                    GlobalVar::timeShareHighLowPoint[2]=list[5].toFloat();
                GlobalVar::mTimeShareChartList.append(info);
            }
        else
            for (int i = 0; i < data.size(); ++i)
            {
                timeShartChartInfo info;
                QStringList list=data.at(i).toString().split(",");
                if (pp<list[2].toFloat())
                    info.direct=2;
                else if (pp>list[2].toFloat())
                    info.direct=1;
                else
                    info.direct=3;
                pp=list[2].toFloat();
                info.time=list[0];
                info.price=per(pp);
                info.vol=list[5].toFloat();
                info.avePrice=per(list[7].toFloat());
                if (list[3].toFloat()>h)
                    h=list[3].toFloat();
                if (list[4].toFloat()<l)
                    l=list[4].toFloat();

                if (list[5].toFloat()>GlobalVar::timeShareHighLowPoint[2])
                    GlobalVar::timeShareHighLowPoint[2]=list[5].toFloat();
                GlobalVar::mTimeShareChartList.append(info);
            }
//        GlobalVar::mTimeShareChartList=timeShareChartList;
        GlobalVar::timeShareHighLowPoint[0]=per(h);
        if (GlobalVar::timeShareHighLowPoint[0]<0)
            GlobalVar::timeShareHighLowPoint[0]=0;
        GlobalVar::timeShareHighLowPoint[1]=per(l);
        if (GlobalVar::timeShareHighLowPoint[1]>0)
            GlobalVar::timeShareHighLowPoint[1]=0;
    }
    m_mutex.unlock();
}

