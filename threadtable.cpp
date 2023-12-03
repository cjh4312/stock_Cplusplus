
#include "threadtable.h"

ThreadTable::ThreadTable(QObject *parent)
    : QObject{parent}
{
    StockInfo info;
    info.code="";
    for (int i=0;i<20;++i)
        GlobalVar::mRisingSpeedList.append(info);
    readMyStock();
}

void ThreadTable::getTableData()
{
//    QByteArray allData;
//    QTime t=QDateTime::currentDateTime().time();
    if (GlobalVar::WhichInterface==1)
    {
        QString fs="m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048";
        GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297"));

        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
            {
                initTableList();
                reFlaseMyStock();
                emit getTableDataFinished();
            }
    }
    else if (GlobalVar::WhichInterface==2)
    {
        GlobalVar::getData(allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs=m:116+t:3,m:116+t:4,m:116+t:1,m:116+t:2&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667966922156"));
        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
            {
                initTableList();
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
        GlobalVar::getData(allData,3.5,QUrl(s));
        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
            {
                initTableList();
                emit getTableDataFinished();
            }
    }
//    qDebug()<<t.msecsTo(QDateTime::currentDateTime().time());
}

void ThreadTable::initTableList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        GlobalVar::upNums=0;
        GlobalVar::downNums=0;
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        StockInfo info;
        if (GlobalVar::WhichInterface==1)
        {
            GlobalVar::mTableListCopy.clear();
            for (int i = 0; i < data.size(); ++i)
            {
                value = data.at(i);
                ceilMap = value.toVariant().toMap();

                info.name = ceilMap.value("f14").toString();
                if (info.name.contains("退"))
                    continue;
                info.code = ceilMap.value("f12").toString();
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
                GlobalVar::mTableListCopy.append(info);
            }
            if (not GlobalVar::isBoard)
                GlobalVar::mTableList=GlobalVar::mTableListCopy;
            for (int i=0;i<20;++i)
                GlobalVar::mRisingSpeedList.replace(i,GlobalVar::mTableListCopy.at(i));
            GlobalVar::sortByColumn(&GlobalVar::mTableListCopy,0,true);
//            GlobalVar::m_risingSpeedModel->setModelData(GlobalVar::mRisingSpeedList,false);
        }
        else
        {
            GlobalVar::mTableList.clear();
            for (int i = 0; i < data.size(); ++i)
            {
                value = data.at(i);
                ceilMap = value.toVariant().toMap();

                info.name = ceilMap.value("f14").toString();
                if (GlobalVar::WhichInterface==5)
                    info.code = ceilMap.value("f13").toString()+"."+ceilMap.value("f12").toString();
                else
                    info.code = ceilMap.value("f12").toString();
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
                GlobalVar::mTableList.append(info);
            }
        }
        GlobalVar::sortByColumn(&GlobalVar::mTableList,GlobalVar::curSortNum,GlobalVar::is_asc);
//        GlobalVar::m_tableModel->setModelData(GlobalVar::mTableList,false);
    }
}

void ThreadTable::readMyStock()
{
    GlobalVar::mMyStockCode=GlobalVar::settings->value("myStock").toStringList();
    StockInfo info;
    for(int i=0;i<GlobalVar::mMyStockCode.count();++i)
    {
        info.code=GlobalVar::mMyStockCode.at(i);
        GlobalVar::mMyStockList.append(info);
    }
}

void ThreadTable::reFlaseMyStock()
{
    int n=GlobalVar::mTableListCopy.count();
    if (n==0)
        return;
    StockInfo info;
    for (int i=0;i<GlobalVar::mMyStockList.count();++i)
    {
        int l = 0;
        int r = n - 1;
        while (l <= r)
        {
            int mid = (l + r) / 2;
            if (GlobalVar::mTableListCopy.at(mid).code == GlobalVar::mMyStockList.at(i).code)
            {
                info=GlobalVar::mTableListCopy.at(mid);
                GlobalVar::mMyStockList.replace(i,info);
                break;
            }
            else if (GlobalVar::mTableListCopy.at(mid).code > GlobalVar::mMyStockList.at(i).code)
                r = mid - 1;
            else
                l = mid + 1;
        }
    }
//    GlobalVar::m_myStockModel->setModelData(GlobalVar::mMyStockList,false);
}

