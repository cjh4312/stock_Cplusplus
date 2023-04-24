
#include "threadcandlechart.h"
#include "globalvar.h"

ThreadCandleChart::ThreadCandleChart(QObject *parent)
    : QObject{parent}
{
    naManager = new QNetworkAccessManager(this);
}

void ThreadCandleChart::getAllCandleChart(QString freq, QString adjustFlag)
{
    if (isRunning)
        return;
    isRunning=true;
    QDate curDay=QDateTime::currentDateTime().date();
//    qDebug()<<curTime.toString("yyyyMMdd");
    QString startDate=curDay.addDays(-1095).toString("yyyyMMdd");
    if (freq=="102")
        startDate=curDay.addDays(-3650).toString("yyyyMMdd");
    else if(freq=="103")
        startDate="19900101";
//    qDebug()<<startDate;
    GlobalVar::getEastData(naManager,allData,2,QUrl("http://push2his.eastmoney.com/api/qt/stock/kline/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61&beg="+startDate+"&end=20500101&ut=fa5fd1943c7b386f172d6893dbfba10b&rtntype=6&secid="+GlobalVar::getComCode()+"&klt="+freq+"&fqt="+adjustFlag));
    initCandleChartList();
    if (not GlobalVar::mCandleChartList.isEmpty())
        emit getCandleChartFinished();
    isRunning=false;
}

void ThreadCandleChart::initCandleChartList()
{
    GlobalVar::candleHighLowPoint[0]={0.0};
    GlobalVar::candleHighLowPoint[1]={100000.0};
    GlobalVar::candleHighLowPoint[2]={0.0};
    GlobalVar::mCandleChartList.clear();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();

        QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();
//        qDebug()<<data.size();
        float MA;
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList list=data.at(i).toString().split(",");
//            qDebug()<<list;
            candleChartInfo info;
            info.time=list[0];
            info.open=list[1].toFloat();
            info.close=list[2].toFloat();
            info.pctChg=list[8].toFloat();
            info.high=list[3].toFloat();
            info.low=list[4].toFloat();
            info.vol=list[5].toFloat();
            info.amount=list[6].toFloat();
            info.turn=list[10].toFloat();
            info.MA5=0;
            info.MA10=0;
            info.MA20=0;
            info.MA60=0;
            info.VMA5=0;
            info.VMA10=0;
//            if (info.high>GlobalVar::candleHighLowPoint[0])
//            {
//                GlobalVar::candleHighLowPoint[0]=info.high;
//                GlobalVar::candleHighLowPoint[3]=j;
//            }
//            if (info.low<GlobalVar::candleHighLowPoint[1])
//            {
//                GlobalVar::candleHighLowPoint[1]=info.low;
//                GlobalVar::candleHighLowPoint[4]=j;
//            }
//            if (info.vol>GlobalVar::candleHighLowPoint[2])
//                GlobalVar::candleHighLowPoint[2]=info.vol;

            MA=0;
            if (i>=4)
            {
                for (int k=i-4;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).close;
                info.MA5=(MA+info.close)/5;
                MA=0;
                for (int k=i-4;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).vol;
                info.VMA5=(MA+info.vol)/5;
            }
            if (i>=9)
            {
                MA=0;
                for (int k=i-9;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).close;
                info.MA10=(MA+info.close)/10;
                MA=0;
                for (int k=i-9;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).vol;
                info.VMA10=(MA+info.vol)/10;
            }
            if (i>=19)
            {
                MA=0;
                for (int k=i-19;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).close;
                info.MA20=(MA+info.close)/20;
            }
            if (i>=59)
            {
                MA=0;
                for (int k=i-59;k<i;++k)
                    MA+=GlobalVar::mCandleChartList.at(k).close;
                info.MA60=(MA+info.close)/60;
            }
            GlobalVar::mCandleChartList.append(info);
        }
//        qDebug()<<GlobalVar::candleHighLowPoint[2];
        float temp;
        int begin=GlobalVar::mCandleChartList.size()-GlobalVar::KRange;
        if (begin<0)
            begin=0;
        for (int i=begin;i<GlobalVar::mCandleChartList.size();++i)
        {
            temp=GlobalVar::mCandleChartList.at(i).high;
            if (temp>GlobalVar::candleHighLowPoint[0])
            {
                GlobalVar::candleHighLowPoint[0]=temp;
                GlobalVar::candleHighLowPoint[3]=i-begin;
            }
            temp=GlobalVar::mCandleChartList.at(i).low;
            if (temp<GlobalVar::candleHighLowPoint[1])
            {
                GlobalVar::candleHighLowPoint[1]=temp;
                GlobalVar::candleHighLowPoint[4]=i-begin;
            }
            temp=GlobalVar::mCandleChartList.at(i).vol;
            if (temp>GlobalVar::candleHighLowPoint[2])
                GlobalVar::candleHighLowPoint[2]=temp;
        }
    }
}

