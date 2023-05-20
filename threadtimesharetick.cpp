
#include "threadtimesharetick.h"
#include "globalvar.h"
ThreadTimeShareTick::ThreadTimeShareTick(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
}

void ThreadTimeShareTick::getBuySellTimeShareTick()
{
    if (isRunning)
        return;
    isRunning=true;
    QByteArray buySellData;
    GlobalVar::getData(buySellData,0.9,QUrl("http://push2.eastmoney.com/api/qt/stock/get?ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=2&volt=2&fields=f43,f44,f45,f46,f47,f48,f58,f60,f108,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+GlobalVar::getComCode()+"&_=1666089246963"));
    if (not buySellData.isEmpty())
    {
        initBuySellList(buySellData);
        if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="6" or GlobalVar::curCode.left(1)=="0")
            findStockArea();
        emit getBuySellFinished();
    }
//    GlobalVar::timeOutFlag[8]=false;
    QByteArray timeShareTickData;
    GlobalVar::getData(timeShareTickData,0.9,QUrl("http://push2.eastmoney.com/api/qt/stock/details/get?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos=-0&secid="+GlobalVar::getComCode()));
    if (not timeShareTickData.isEmpty())
    {
        initTimeShareTickList(timeShareTickData);
        emit getTimeShareTickFinished();
    }
//    GlobalVar::timeOutFlag[7]=false;
    isRunning=false;
}

void ThreadTimeShareTick::initBuySellList(const QByteArray &allData)
{
    QString price[]={"f31","f33","f35","f37","f39","f19","f17","f15","f13","f11"};
    QString nums[]={"f32","f34","f36","f38","f40","f20","f18","f16","f14","f12"};
    QString baseInfo[]={"f43","f170","f168","f48","f84","f116","f46","f44","f45","f47","f85","f108","f162"};
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        GlobalVar::preClose=jsonObject.value("data").toObject().value("f60").toDouble();
        GlobalVar::curName=jsonObject.value("data").toObject().value("f58").toString();
        for (int i=0;i<10;++i)
        {
            GlobalVar::buySellPrice[i]=jsonObject.value("data").toObject().value(price[i]).toDouble();
            GlobalVar::buySellNum[i]=int(jsonObject.value("data").toObject().value(nums[i]).toDouble()+0.5);
        }
        for (int i=0;i<13;++i)
            GlobalVar::baseInfoData[i]=jsonObject.value("data").toObject().value(baseInfo[i]).toDouble();
    }
    if (GlobalVar::WhichInterface!=2 and GlobalVar::WhichInterface!=5)
    {
        QByteArray allData;
        GlobalVar::getData(allData,0.5,QUrl("https://datacenter-web.eastmoney.com/api/data/v1/get?sortColumns=REPORTDATE&sortTypes=-1&pageSize=1&pageNumber=1&columns=ALL&filter=(SECURITY_CODE%3D%22"+GlobalVar::curCode+"%22)&reportName=RPT_LICO_FN_CPD"));
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("result").toObject().value("data").toArray();
            QJsonValue value = data.at(0);
            QVariantMap ceilMap = value.toVariant().toMap();
            GlobalVar::baseInfoData[11]=ceilMap.value("BASIC_EPS").toDouble();
            GlobalVar::EPSReportDate=ceilMap.value("DATEMMDD").toString();
        }
    }
}

void ThreadTimeShareTick::initTimeShareTickList(const QByteArray &allData)
{
    GlobalVar::mTimeShareTickList.clear();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
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
            GlobalVar::mTimeShareTickList.append(info);
        }
    }
}
//显示股票的区域和行业
void ThreadTimeShareTick::findStockArea()
{
    QFile file(GlobalVar::currentPath+"/list/stock_list.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QStringList Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);

        int l = 0;
        int r = Data.count() - 1;

        while (l <= r)
        {
            int mid = (l + r) / 2;
            QStringList strLine = Data.at(mid).split(",");
            if(strLine[1]==GlobalVar::curCode)
            {
                GlobalVar::curName=strLine[2]+"("+strLine[3]+")"+"--"+strLine[4];
                break;
            }
            else if (strLine[1]>GlobalVar::curCode)
                r = mid - 1;
            else
                l = mid + 1;
        }
    }
}

