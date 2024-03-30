
#include "threadtimesharechart.h"

ThreadTimeShareChart::ThreadTimeShareChart(QObject *parent)
    : QObject{parent}
{
    // request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
}

void ThreadTimeShareChart::getSSEData()
{
    QString url="https://push2his.eastmoney.com/api/qt/stock/trends2/sse?mpi=2000&fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893";
    QByteArray* qByteArray=new QByteArray();
    QString preCode=GlobalVar::curCode;
    QNetworkRequest request;
    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
    request.setUrl(QUrl(url));
    QNetworkReply *reply= naManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        disconnect(reply);
        reply->deleteLater();
        delete qByteArray;
        naManager->deleteLater();
    });
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        if (GlobalVar::curCode!=preCode or reset)
        {
            reset=false;
            reply->abort();
        }
        else
        {
            int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (statusCode == 200)
            {
                mRetries=0;
                QByteArray tempData=reply->readAll();
                if (tempData.contains("data:"))
                {
                    if (tempData.contains("\"data\":{\""))
                    {
                        if (tempData.mid(tempData.size()-2,2)=="\n\n")
                        {
                            allData=tempData;
                            initSSETimeShareChartList();
                            emit getTimeShareChartFinished();
                        }
                        else
                            qByteArray->append(tempData);
                    }
                }
                else
                {
                    qByteArray->append(tempData);
                    QByteArray tempByteArray=qByteArray->data();
                    if (tempByteArray.mid(tempByteArray.size()-2,2)=="\n\n")
                    {
                        allData=tempByteArray;
                        initSSETimeShareChartList();
                        emit getTimeShareChartFinished();
                        qByteArray->clear();
                    }
                }
            }
            else
                if(mRetries < MAX_RETRIES)
                {
                    mRetries++;
                    getSSEData();
                }

        }
    });
}

void ThreadTimeShareChart::getAllTimeShareChart(bool r)
{
    if (preGCode==GlobalVar::curCode and not r)
        return;

    reset=r;
    h=0.0;
    l=10000000.0;
    getSSEData();


    // GlobalVar::getData(allData,2,QUrl("https://push2his.eastmoney.com/api/qt/stock/trends2/get?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13&fields2=f51,f52,f53,f54,f55,f56,f57,f58&ut=fa5fd1943c7b386f172d6893dbfba10b&iscr=0&ndays=1&secid="+GlobalVar::getComCode()+"&_=1666401553893"));
    // if (GlobalVar::timeOutFlag[6])
    //     GlobalVar::timeOutFlag[6]=false;
    // else
    // {
    //     initTimeShareChartList();
    //     emit getTimeShareChartFinished();
    // }
}

void ThreadTimeShareChart::initTimeShareChartList()
{
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

        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
        float h=0.0;
        float l=100000.0;
        float vl=0.0;
        GlobalVar::timeShareHighLowPoint[2]=0;
        float pp=GlobalVar::preClose;
        QList<timeShartChartInfo> mTimeShareChartList;
        timeShartChartInfo info;
        QStringList list;
        if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
            for (int i = 0; i < data.size(); ++i)
            {
                list=data.at(i).toString().split(",");
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

                if (list[5].toFloat()>vl)
                    vl=list[5].toFloat();
                mTimeShareChartList.append(info);
            }
        else
            for (int i = 0; i < data.size(); ++i)
            {
                list=data.at(i).toString().split(",");
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

                if (list[5].toFloat()>vl)
                    vl=list[5].toFloat();
                mTimeShareChartList.append(info);
            }
        GlobalVar::timeShareHighLowPoint[0]=per(h);
        GlobalVar::timeShareHighLowPoint[2]=vl;
        if (GlobalVar::timeShareHighLowPoint[0]<0)
            GlobalVar::timeShareHighLowPoint[0]=0;
        GlobalVar::timeShareHighLowPoint[1]=per(l);
        if (GlobalVar::timeShareHighLowPoint[1]>0)
            GlobalVar::timeShareHighLowPoint[1]=0;
        GlobalVar::mTimeShareChartList=mTimeShareChartList;
    }
}

void ThreadTimeShareChart::initSSETimeShareChartList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData.mid(6,allData.size()-6), &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("trends").toArray();
        timeShartChartInfo info;
        QStringList list;
        if (GlobalVar::curCode!=preGCode or reset)
        {
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
            GlobalVar::timeShareHighLowPoint[2]=0;
            pp=GlobalVar::preClose;
            GlobalVar::trendsTotal=jsonObject.value("data").toObject().value("trendsTotal").toInt();
            QList<timeShartChartInfo> mTimeShareChartList;
            for (int i = 0; i < data.size(); ++i)
            {
                if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
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
                    if (pp<list[2].toFloat())
                        info.direct=2;
                    else if (pp>list[2].toFloat())
                        info.direct=1;
                    else
                        info.direct=3;
                    pp=list[2].toFloat();
                    mTimeShareChartList.append(info);
                }
                else
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.vol=list[5].toFloat();
                    info.avePrice=per(list[7].toFloat());
                    if (list[3].toFloat()>h)
                        h=list[3].toFloat();
                    if (list[4].toFloat()<l)
                        l=list[4].toFloat();
                    if (list[5].toFloat()>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=list[5].toFloat();
                    if (pp<list[2].toFloat())
                        info.direct=2;
                    else if (pp>list[2].toFloat())
                        info.direct=1;
                    else
                        info.direct=3;
                    pp=list[2].toFloat();
                    mTimeShareChartList.append(info);
                }
            }
            GlobalVar::mTimeShareChartList=mTimeShareChartList;
        }
        else
        {
            float backPP=0.0;
            if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
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
                    if (info.time!=GlobalVar::mTimeShareChartList.at(GlobalVar::mTimeShareChartList.size()-1).time)
                    {
                        pp=backPP;
                        if (pp<list[2].toFloat())
                            info.direct=2;
                        else if (pp>list[2].toFloat())
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=pp=list[2].toFloat();
                        GlobalVar::mTimeShareChartList.append(info);
                    }
                    else
                    {
                        if (pp<list[2].toFloat())
                            info.direct=2;
                        else if (pp>list[2].toFloat())
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=list[2].toFloat();
                        GlobalVar::mTimeShareChartList.replace(GlobalVar::mTimeShareChartList.size()-1,info);
                    }
                }
            else
                for (int i = 0; i < data.size(); ++i)
                {
                    list=data.at(i).toString().split(",");
                    info.time=list[0];
                    info.price=per(list[2].toFloat());
                    info.avePrice=per(list[7].toFloat());
                    info.vol=list[5].toFloat();
                    if (list[3].toFloat()>h)
                        h=list[3].toFloat();
                    if (list[4].toFloat()<l)
                        l=list[4].toFloat();
                    if (list[5].toFloat()>GlobalVar::timeShareHighLowPoint[2])
                        GlobalVar::timeShareHighLowPoint[2]=list[5].toFloat();
                    if (info.time!=GlobalVar::mTimeShareChartList.at(GlobalVar::mTimeShareChartList.size()-1).time)
                    {
                        pp=backPP;
                        if (pp<list[2].toFloat())
                            info.direct=2;
                        else if (pp>list[2].toFloat())
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=pp=list[2].toFloat();
                        GlobalVar::mTimeShareChartList.append(info);
                    }
                    else
                    {
                        if (pp<list[2].toFloat())
                            info.direct=2;
                        else if (pp>list[2].toFloat())
                            info.direct=1;
                        else
                            info.direct=3;
                        backPP=list[2].toFloat();
                        GlobalVar::mTimeShareChartList.replace(GlobalVar::mTimeShareChartList.size()-1,info);
                    }
                }
        }
        GlobalVar::timeShareHighLowPoint[0]=per(h);
        if (GlobalVar::timeShareHighLowPoint[0]<0)
            GlobalVar::timeShareHighLowPoint[0]=0;
        GlobalVar::timeShareHighLowPoint[1]=per(l);
        if (GlobalVar::timeShareHighLowPoint[1]>0)
            GlobalVar::timeShareHighLowPoint[1]=0;
        preGCode=GlobalVar::curCode;
    }
}
