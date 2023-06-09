
#include "threadtable.h"
#include "globalvar.h"

ThreadTable::ThreadTable(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
    readMyStock();
}

void ThreadTable::getTableData()
{
    QByteArray allData;
    if (GlobalVar::WhichInterface==1)
    {
        QString fs="m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048";
        GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297"));
        if (not allData.isEmpty())
        {
            initTableList(allData);
            emit getTableDataFinished();
            reFlaseMyStock();
        }
    }
    else if (GlobalVar::WhichInterface==2)
    {
        GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs=m:116+t:3,m:116+t:4,m:116+t:1,m:116+t:2&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667966922156"));
        if (not allData.isEmpty())
        {
            initTableList(allData);
            emit getTableDataFinished();
        }
    }
    else if (GlobalVar::WhichInterface==5)
    {
        QString fs;
        if (GlobalVar::isUsZhStock)
            fs = "b:mk0201";
        else
            fs = "m:105,m:106,m:107";
        QString s="http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=20000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f13,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667962034515";
        GlobalVar::getData(allData,3,QUrl(s));
        if (not allData.isEmpty())
        {
            initTableList(allData);
            emit getTableDataFinished();
        }
    }
}

void ThreadTable::initTableList(const QByteArray &allData)
{
    if (not GlobalVar::isBoard)
        GlobalVar::mTableList.clear();
    if (GlobalVar::WhichInterface==1)
    {
        GlobalVar::mTableListCopy.clear();
        GlobalVar::mRisingSpeedList.clear();
    }
    GlobalVar::upNums=0;
    GlobalVar::downNums=0;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        int n=0;
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();

            StockInfo info;
            info.name = ceilMap.value("f14").toString();
            if (info.name.contains("退"))
                continue;
            info.code = ceilMap.value("f12").toString();
            if (GlobalVar::WhichInterface==5)
                info.code = ceilMap.value("f13").toString()+"."+ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toFloat();
            info.pctChg=ceilMap.value("f3").toFloat();
            if (info.pctChg>0)
                GlobalVar::upNums+=1;
            else if (info.pctChg<0)
                GlobalVar::downNums+=1;
            info.turn=ceilMap.value("f8").toFloat();
            info.amount=ceilMap.value("f6").toFloat();
            info.velocity = ceilMap.value("f22").toFloat();
            info.pe = ceilMap.value("f9").toFloat();
            info.totalValue = ceilMap.value("f20").toFloat();
            info.circulatedValue=ceilMap.value("f21").toFloat();
            info.pctYear=ceilMap.value("f25").toFloat();
            info.pctSixty=ceilMap.value("f24").toFloat();
            info.volume = ceilMap.value("f5").toFloat();
            info.high = ceilMap.value("f15").toFloat();
            info.low = ceilMap.value("f16").toFloat();
            info.open=ceilMap.value("f17").toFloat();
            info.preClose=ceilMap.value("f18").toFloat();
            if (not GlobalVar::isBoard)
                GlobalVar::mTableList.append(info);
            if (GlobalVar::WhichInterface==1)
            {
                GlobalVar::mTableListCopy.append(info);
                if (n<=19)
                {
                    GlobalVar::mRisingSpeedList.append(info);
                    n+=1;
                }
            }
        }
        if (GlobalVar::WhichInterface==1)
            GlobalVar::sortByColumn(&GlobalVar::mTableListCopy,0,true);
        GlobalVar::sortByColumn(&GlobalVar::mTableList,GlobalVar::curSortNum,GlobalVar::is_asc);
    }
}

void ThreadTable::readMyStock()
{
    GlobalVar::mMyStockCode=GlobalVar::settings->value("myStock").toStringList();
    for(int i=0;i<GlobalVar::mMyStockCode.count();++i)
    {
        StockInfo info;
        info.code=GlobalVar::mMyStockCode[i];
        GlobalVar::mMyStockList.append(info);
    }
}

void ThreadTable::reFlaseMyStock()
{
    int n=GlobalVar::mTableListCopy.count();
    if (n==0)
        return;
    for (int i=0;i<GlobalVar::mMyStockList.count();++i)
    {
        int l = 0;
        int r = n - 1;
        while (l <= r)
        {
            int mid = (l + r) / 2;
            if (GlobalVar::mTableListCopy.at(mid).code == GlobalVar::mMyStockList.at(i).code)
            {
                StockInfo info=GlobalVar::mTableListCopy.at(mid);
                GlobalVar::mMyStockList.replace(i,info);
                break;
            }
            else if (GlobalVar::mTableListCopy.at(mid).code > GlobalVar::mMyStockList.at(i).code)
                r = mid - 1;
            else
                l = mid + 1;
        }
    }
}

