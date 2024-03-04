
#include "threadtable.h"

ThreadTable::ThreadTable(QObject *parent)
    : QObject{parent}
{
    StockInfo info;
    info.code="";
    for (int i=0;i<risingSpeedSize;++i)
        GlobalVar::mRisingSpeedList.append(info);
    readMyStock();
}

void ThreadTable::getTableData()
{
//    QByteArray allData;
//    QTime t=QDateTime::currentDateTime().time();
    QString fs="";
    if (GlobalVar::WhichInterface==1)
    {
        fs="m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048";
        GlobalVar::getData(allData,3,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297"));

        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
            {
                initTableList();
                reFlaseMyStock();
                emit getTableDataFinished();
                if (GlobalVar::isZhMarketDay(QDateTime::currentDateTime()) or isFirst)
                {
                    getEastFundFlow();
                    if (not GlobalVar::mFundFlowList.isEmpty())
                        emit getBlockFinished();
                    isFirst=false;
                }
            }
    }
    else
    {
        if (GlobalVar::WhichInterface==2)
            fs="m:116+t:3,m:116+t:4,m:116+t:1,m:116+t:2";
        else if (GlobalVar::WhichInterface==5)
        {
            if (GlobalVar::isUsZhStock)
                fs = "b:mk0201";
            else
                fs = "m:105,m:106,m:107";
        }
        else if (GlobalVar::WhichInterface==6)
            fs="m:155+t:1,m:155+t:2,m:155+t:3,m:156+t:1,m:156+t:2,m:156+t:5,m:156+t:6,m:156+t:7,m:156+t:8";
        GlobalVar::getData(allData,3.5,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f13,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667966922156"));
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
            for (int i=0;i<risingSpeedSize;++i)
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
                if (GlobalVar::WhichInterface==5 or GlobalVar::WhichInterface==6)
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
    if (GlobalVar::mTableListCopy.isEmpty())
        return;
    StockInfo info;
    for (int i=0;i<GlobalVar::mMyStockList.count();++i)
    {
        info=GlobalVar::findStock(GlobalVar::mMyStockList.at(i).code);
        GlobalVar::mMyStockList.replace(i,info);
    }
    //    GlobalVar::m_myStockModel->setModelData(GlobalVar::mMyStockList,false);
}

void ThreadTable::getEastFundFlow()
{
    QString url[]={"https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=50&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A1&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13",
                     "https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=500&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A3&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13",
                     "https://push2.eastmoney.com/api/qt/clist/get?fid=f62&po=1&pz=100&pn=1&np=1&fltt=2&invt=2&ut=b2884a393a59ad64002292a3e90d46a5&fs=m%3A90+t%3A2&fields=f12%2Cf14%2Cf2%2Cf3%2Cf62%2Cf184%2Cf66%2Cf69%2Cf72%2Cf75%2Cf78%2Cf81%2Cf84%2Cf87%2Cf204%2Cf205%2Cf124%2Cf1%2Cf13"};
    QStringList FundFlowCol;
    FundFlowCol<<"名称"<<"涨跌幅"<<"主力净额"<<"主力净占比"<<"超大单净额"<<"超大单净占比"<<"大单净额"
                <<"大单净占比"<<"中单净额"<<"中单净占比"<<"小单净额"<<"小单净占比"<<"主力净流入最大股"<<"CODE";
    GlobalVar::mFundFlowList.clear();
    for (int i=0;i<3;++i)
    {
        QByteArray allData;
        GlobalVar::getData(allData,1.5,QUrl(url[i]));
        QString v1="f3",v2="f62",v3="f184",v4="f66",v5="f69",v6="f72",v7="f75",
            v8="f78",v9="f81",v10="f84",v11="f87",v12="f204",v13="f12";
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = doc.object();
            QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();

            for (int i = 0; i < data.size(); ++i)
            {
                QJsonValue value = data.at(i);
                QVariantMap ceilMap = value.toVariant().toMap();
                QStringList dataList;
                QString name=ceilMap.value("f14").toString();
                if (name.contains("昨日"))
                    continue;
                dataList<<name<<ceilMap.value(v1).toString()
                         <<ceilMap.value(v2).toString()<<ceilMap.value(v3).toString()
                         <<ceilMap.value(v4).toString()<<ceilMap.value(v5).toString()
                         <<ceilMap.value(v6).toString()<<ceilMap.value(v7).toString()
                         <<ceilMap.value(v8).toString()<<ceilMap.value(v9).toString()
                         <<ceilMap.value(v10).toString()<<ceilMap.value(v11).toString()
                         <<ceilMap.value(v12).toString()<<ceilMap.value(v13).toString();
                GlobalVar::mFundFlowList.append(dataList);
            }
        }
    }
    std::sort(GlobalVar::mFundFlowList.begin(),GlobalVar::mFundFlowList.end(),[](QStringList a,QStringList b){
        return a[1].toFloat()>b[1].toFloat();
    });
}

