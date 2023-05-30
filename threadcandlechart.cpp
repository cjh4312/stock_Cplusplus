
#include "threadcandlechart.h"
#include "globalvar.h"

ThreadCandleChart::ThreadCandleChart(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
}

void ThreadCandleChart::getAllCandleChart(QString freq, QString adjustFlag,bool isFirst)
{
    if (isRunning)
        return;
    isRunning=true;
    QString startDate;
    if (isFirst and freq=="101")
        startDate=QDateTime::currentDateTime().addDays(-KRANGE*7/3).toString("yyyyMMdd");
    else
        startDate="19900101";
    QByteArray allData;
    GlobalVar::getData(allData,2,QUrl("http://push2his.eastmoney.com/api/qt/stock/kline/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61&beg="+startDate+"&end=20500101&ut=fa5fd1943c7b386f172d6893dbfba10b&rtntype=6&secid="+GlobalVar::getComCode()+"&klt="+freq+"&fqt="+adjustFlag));
    if(not allData.isEmpty())
    {
        initCandleChartList(allData);
        emit getCandleChartFinished();
    }
//    GlobalVar::timeOutFlag[0]=false;
    isRunning=false;
}

void ThreadCandleChart::initCandleChartList(const QByteArray &allData)
{
    GlobalVar::mCandleChartList.clear();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();

        QJsonArray data=jsonObject.value("data").toObject().value("klines").toArray();
        float MA;
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList list=data.at(i).toString().split(",");
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
    }
}

