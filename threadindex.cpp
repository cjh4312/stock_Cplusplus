
#include "threadindex.h"

ThreadIndex::ThreadIndex(QObject *parent)
    : QObject{parent}
{
    IndexInfo info;
    info.name="name";
    for (int i=0;i<20;++i)
        GlobalVar::mIndexList.append(info);
}

void ThreadIndex::getAllIndex()
{
   // QTime t=QDateTime::currentDateTime().time();
    GlobalVar::getData(indexData,2,QUrl("https://push2.eastmoney.com/api/qt/ulist.np/get?&pz=20&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&fields=f14,f2,f3&secids=1.000001%2C0.399001%2C0.399006%2C100.HSI%2C100.N225%2C100.KS11%2C1.000688%2c100.TWII%2C100.SENSEX%2C100.DJIA%2C100.SPX%2C100.NDX%2C100.SX5E%2C100.GDAXI%2C100.RTS%2C100.FTSE%2C100.FCHI%2C100.AS51%2C103.YM00Y%2C104.CN00Y&_=1662857186403"));
    if (GlobalVar::timeOutFlag[1])
        GlobalVar::timeOutFlag[1]=false;
    else
        {
            initIndexList();
            emit getIndexFinished();
        }
    if (GlobalVar::isZhMarketDay(QDateTime::currentDateTime()) or isFirst)
    {
        getEastFundFlow();
        if (not GlobalVar::mFundFlowList.isEmpty())
            emit getBlockFinished();
        isFirst=false;
    }
   // qDebug()<<t.msecsTo(QDateTime::currentDateTime().time());
}

void ThreadIndex::initIndexList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(indexData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        IndexInfo info;
        for (int i = 0; i < data.size(); ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toString();
            info.pctChg=ceilMap.value("f3").toString();
            GlobalVar::mIndexList.replace(i,info);
        }
    }
}

void ThreadIndex::getEastFundFlow()
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
        GlobalVar::getData(allData,1,QUrl(url[i]));
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

