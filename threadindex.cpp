
#include "threadindex.h"
#include "globalvar.h"

ThreadIndex::ThreadIndex(QObject *parent)
    : QObject{parent}
{
    IndexInfo info;
    for (int i=0;i<20;++i)
    {
        info.code="0000";
        info.name="name";
        info.close="1";
        info.pctChg="0.00";
        GlobalVar::mIndexList.append(info);
    }
}

void ThreadIndex::getAllIndex()
{
    GlobalVar::getData(indexData,1.8,QUrl("http://push2.eastmoney.com/api/qt/ulist.np/get?fid=f14&pi=0&pz=40&po=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&fields=f14,f12,f2,f3&np=1&secids=1.000001%2C0.399001%2C0.399006%2C100.HSI%2C100.N225%2C100.KS11%2C1.000688%2c100.TWII%2C100.SENSEX%2C100.DJIA%2C100.SPX%2C100.NDX%2C100.SX5E%2C100.GDAXI%2C100.RTS%2C100.FTSE%2C100.FCHI%2C100.AS51&_=1662857186403"));
    if (GlobalVar::timeOutFlag[1])
        GlobalVar::timeOutFlag[1]=false;
    else
        {
            GlobalVar::getData(futuresData,2,QUrl("https://futsseapi.eastmoney.com/list/block?orderBy=name&sort=desc&pageSize=20&pageIndex=0&token=58b2fa8f54638b60b87d69b31969089c&field=%2Cname%2Cp%2Czdf&blockName=financial&_=1700020202870"));
            if (GlobalVar::timeOutFlag[2])
                GlobalVar::timeOutFlag[2]=false;
            else
            {
                initIndexList();
                initFuturesList();
                if (GlobalVar::mIndexList.count()>=20)
                    emit getIndexFinished();
            }
        }
}

void ThreadIndex::initIndexList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(indexData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QList<IndexInfo> indexList;
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        IndexInfo info;
        for (int i = 0; i < data.size(); ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            info.code = ceilMap.value("f12").toString();
            if (i>2 && i<16)
                info.code = "100."+ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toString();
            info.pctChg=ceilMap.value("f3").toString();
            indexList.append(info);
        }
        GlobalVar::mIndexList=indexList;
    }
}

void ThreadIndex::initFuturesList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(futuresData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("list").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        IndexInfo info;
        for (int i = 0; i < data.size(); ++i)
        {
            value = data.at(i);
            ceilMap = value.toVariant().toMap();
            info.name = ceilMap.value("name").toString();
            if (info.name=="A50期指当月连续" || info.name=="小型道指当月连续")
            {
                info.close = ceilMap.value("p").toString();
                info.pctChg=ceilMap.value("zdf").toString();
                GlobalVar::mIndexList.append(info);
            }
        }
    }
}

