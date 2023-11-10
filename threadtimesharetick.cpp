
#include "threadtimesharetick.h"
#include "globalvar.h"

ThreadTimeShareTick::ThreadTimeShareTick(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
    QFile file(GlobalVar::currentPath+"/list/stock_list.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QStringList data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
        for (int i=0;i<data.count();++i)
        {
            QStringList temp= data.at(i).split(",");
            areaData.append(temp);
        }
        std::sort(areaData.begin(),areaData.end(),[](QStringList a,QStringList b){
            return a[1]<b[1];
        });
    }
    file.close();
}

void ThreadTimeShareTick::getBuySellTimeShareTick()
{
    QByteArray buySellData;
    QByteArray timeShareTickData;

    GlobalVar::getData(buySellData,0.9,QUrl("http://push2.eastmoney.com/api/qt/stock/get?ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=2&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f58,f60,f62,f108,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+GlobalVar::getComCode()+"&_=1666089246963"));

    if (GlobalVar::timeOutFlag[8])
        GlobalVar::timeOutFlag[8]=false;
    else
        {
            initBuySellList(buySellData);
            QString l=GlobalVar::curCode.left(1);
            if (l=="3" or l=="6" or l=="0")
                findStockArea();
            emit getBuySellFinished();
        }

    GlobalVar::getData(timeShareTickData,0.9,QUrl("http://push2.eastmoney.com/api/qt/stock/details/get?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos=-0&secid="+GlobalVar::getComCode()));
    if (GlobalVar::timeOutFlag[7])
            GlobalVar::timeOutFlag[7]=false;
    else
    {
            initTimeShareTickList(timeShareTickData);
            emit getTimeShareTickFinished();
    }


//    if (preGCode==GlobalVar::curCode)
//        return;
//    QString url="http://push2.eastmoney.com/api/qt/stock/sse?ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=2&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f58,f60,f62,f108,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+GlobalVar::getComCode()+"&_=1666089246963";
//    getSSEdata(1,url);
//    url="http://push2.eastmoney.com/api/qt/stock/details/sse?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos=-0&secid="+GlobalVar::getComCode();
//    GlobalVar::mTimeShareTickList.clear();
//    getSSEdata(2,url);
}

void ThreadTimeShareTick::getSSEdata(int nums,QString url)
{
    QByteArray* qByteArray=new QByteArray();
    QString preCode=GlobalVar::curCode;
    preGCode=preCode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
    request.setUrl(QUrl(url));
    QNetworkAccessManager *naManager =new QNetworkAccessManager();
    QNetworkReply *reply= naManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){
        disconnect(reply);
        reply->deleteLater();
        delete qByteArray;
        naManager->deleteLater();
    });
    connect(reply, &QNetworkReply::readyRead, this, [=]()mutable{
        if (GlobalVar::curCode!=preCode)
            reply->abort();
        else
        {
            QByteArray allData=reply->readAll();
            if (allData.contains("data:"))
            {
                if (allData.contains("\"data\":{\""))
                {
                    if (allData.mid(allData.size()-2,2)=="\n\n")
                    {
                        if (nums==1)
                        {
                            initBuySellList(allData.mid(6,allData.size()-8));
                            QString l=GlobalVar::curCode.left(1);
                            if (l=="3" or l=="6" or l=="0")
                                findStockArea();
                            emit getBuySellFinished();
                        }
                        else
                        {
                            initTimeShareTickList(allData.mid(6,allData.size()-8));
                            emit getTimeShareTickFinished();
                        }
                    }
                    else
                        qByteArray->append(allData);
                }
            }
            else
            {
                qByteArray->append(allData);
                QByteArray tempByteArray=qByteArray->data();
//                qDebug()<<tempByteArray;
                if (tempByteArray.mid(tempByteArray.size()-2,2)=="\n\n")
                {
                    if (nums==1)
                    {
                        initBuySellList(tempByteArray.mid(6,tempByteArray.size()-8));
                        QString l=GlobalVar::curCode.left(1);
                        if (l=="3" or l=="6" or l=="0")
                            findStockArea();
                        emit getBuySellFinished();
                    }
                    else
                    {
                        initTimeShareTickList(tempByteArray.mid(6,tempByteArray.size()-8));
                        emit getTimeShareTickFinished();
                    }
                }
            }
        }
    });
}

void ThreadTimeShareTick::initBuySellList(QByteArray buySellData)
{
    QJsonParseError *jsonError=new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(buySellData, jsonError);
    if (jsonError->error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        if (buySellData.contains("f60"))
            GlobalVar::preClose=jsonObject.value("data").toObject().value("f60").toDouble();
        if (buySellData.contains("f58"))
            GlobalVar::curName=jsonObject.value("data").toObject().value("f58").toString();
        for (int i=0;i<10;++i)
        {
            if (buySellData.contains(price[i].toUtf8()))
                GlobalVar::buySellPrice[i]=jsonObject.value("data").toObject().value(price[i]).toDouble();
            if (buySellData.contains(nums[i].toUtf8()))
                GlobalVar::buySellNum[i]=int(jsonObject.value("data").toObject().value(nums[i]).toDouble()+0.5);

            }
        for (int i=0;i<14;++i)
        {
            if (buySellData.contains(baseInfo[i].toUtf8()))
                GlobalVar::baseInfoData[i]=jsonObject.value("data").toObject().value(baseInfo[i]).toDouble();
            if (i==13 and (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5) and
                    buySellData.contains("f164"))
                GlobalVar::baseInfoData[i]=jsonObject.value("data").toObject().value("f164").toDouble();
        }
        if (buySellData.contains("f55"))
            GlobalVar::baseInfoData[12]=jsonObject.value("data").toObject().value("f55").toDouble();
        GlobalVar::EPSReportDate="每股收益";
        GlobalVar::PEName="市盈率";
        if ((GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==4) and
            GlobalVar::curCode.left(1)!="1" and GlobalVar::curCode.left(3)!="399" and
            GlobalVar::curCode.length()!=5 and buySellData.contains("f62"))
        {
            int num=jsonObject.value("data").toObject().value("f62").toInt();
            QString n[4]={"一","二","三","四",};
            GlobalVar::EPSReportDate="收益("+n[num-1]+")";
            GlobalVar::PEName="PE(动)";
        }
    }
}

void ThreadTimeShareTick::initTimeShareTickList(QByteArray timeShareTickData)
{
    QJsonParseError *jsonError=new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(timeShareTickData, jsonError);
    if (jsonError->error == QJsonParseError::NoError)
    {
        QList<timeShareTickInfo> timeShareTickList;
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("details").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QStringList list=data.at(i).toString().split(",");

            timeShareTickInfo info;
            info.time=list[0];
            info.price=list[1].toFloat();
            info.nums=list[2].toInt();
            info.d=list[4].toInt();
            info.tick=list[3].toInt();
            timeShareTickList.append(info);
        }
        GlobalVar::mTimeShareTickList=timeShareTickList;
    }
}
//显示股票的区域和行业
void ThreadTimeShareTick::findStockArea()
{
    int l = 0;
    int r = areaData.count() - 1;

    while (l <= r)
    {
        int mid = (l + r) / 2;
//        QStringList strLine = areaData.at(mid).split(",");
        QStringList s=areaData.at(mid);

        if(s[1]==GlobalVar::curCode)
        {
            if (s[3]!="")
                GlobalVar::curName=s[2]+"("+s[3]+")"+"--"+s[4];
            break;
        }
        else if (s[1]>GlobalVar::curCode)
            r = mid - 1;
        else
            l = mid + 1;
    }
}

