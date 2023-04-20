
#include "threadtimesharechart.h"
#include "globalvar.h"

ThreadTimeShareChart::ThreadTimeShareChart(QObject *parent)
    : QObject{parent}
{
    naManager = new QNetworkAccessManager(this);
}

void ThreadTimeShareChart::getAllTimeShareChart()
{
    if (isRunning)
        return;
    isRunning=true;

//    if (GlobalVar::WhichInterface==1 or preInterface==1)
//        GlobalVar::getEastData(naManager,allData,1.5,QUrl("http://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&secid="+GlobalVar::getComCode()+"&ndays=5&iscr=0&iscca=0"));
//    else
//    {
//        QString code;
//        if (GlobalVar::WhichInterface==5 or preInterface==5)
//            code=GlobalVar::curCode;
//        else if (GlobalVar::WhichInterface==2 or preInterface==2)
//            code="116."+GlobalVar::curCode;
//        GlobalVar::getEastData(naManager,allData,1.5,QUrl("https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+code+"&_=1666401553893"));
//    }
    GlobalVar::getEastData(naManager,allData,1.5,QUrl("https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893"));
    initTimeShareChartList();
    if (not GlobalVar::mTimeShareChartList.isEmpty())
        emit getTimeShareChartFinished();
    isRunning=false;
}

void ThreadTimeShareChart::initTimeShareChartList()
{
    GlobalVar::mTimeShareChartList.clear();
    GlobalVar::timeShareHighLowPoint[0]={0.0};
    GlobalVar::timeShareHighLowPoint[1]={100000.0};
    GlobalVar::timeShareHighLowPoint[2]={0.0};
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
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
        //        qDebug()<<per(a,GlobalVar::preClose)<<per(b,GlobalVar::preClose);
        //        qDebug()<<GlobalVar::preClose<<GlobalVar::timeShareHighLowPoint[3]<<GlobalVar::timeShareHighLowPoint[4];

        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
//        if (GlobalVar::WhichInterface==1)
//            trendsTotal/=5;
//        qDebug()<<data.size();
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList list=data.at(i).toString().split(",");
            timeShartChartInfo info;
            info.time=list[0];
            info.price=per(list[2].toFloat());
            info.vol=list[5].toFloat();
            info.avePrice=per(list[7].toFloat());
            if (list[2].toFloat()>list[1].toFloat())
                info.direct=2;
            else if (list[2].toFloat()<list[1].toFloat())
                info.direct=1;
            else
                info.direct=3;
            if (list[3].toFloat()>GlobalVar::timeShareHighLowPoint[0])
                GlobalVar::timeShareHighLowPoint[0]=list[3].toFloat();
            if (list[4].toFloat()<GlobalVar::timeShareHighLowPoint[1])
                GlobalVar::timeShareHighLowPoint[1]=list[4].toFloat();
            if (list[5].toFloat()>GlobalVar::timeShareHighLowPoint[2])
                GlobalVar::timeShareHighLowPoint[2]=list[5].toFloat();
            GlobalVar::mTimeShareChartList.append(info);
        }
        //        qDebug()<<GlobalVar::timeShareHighLowPoint[1];
        GlobalVar::timeShareHighLowPoint[0]=per(GlobalVar::timeShareHighLowPoint[0]);
        GlobalVar::timeShareHighLowPoint[1]=per(GlobalVar::timeShareHighLowPoint[1]);
        //        qDebug()<<GlobalVar::timeShareHighLowPoint[0]<<GlobalVar::timeShareHighLowPoint[1]<<GlobalVar::timeShareHighLowPoint[2]<<GlobalVar::timeShareHighLowPoint[3]<<GlobalVar::timeShareHighLowPoint[4];
        //        for (int i = 0; i < GlobalVar::mTimeShareChartList.size(); ++i)
        //        {
        //            qDebug()<<GlobalVar::mTimeShareChartList.at(i).time
        //                <<GlobalVar::mTimeShareChartList.at(i).price
        //                <<GlobalVar::mTimeShareChartList.at(i).vol
        //                <<GlobalVar::mTimeShareChartList.at(i).avePrice;
        //        }
    }
}

