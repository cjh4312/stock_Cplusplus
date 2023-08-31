
#include "threadindex.h"
#include "globalvar.h"

ThreadIndex::ThreadIndex(QObject *parent)
    : QObject{parent}
{
//    naManager = new QNetworkAccessManager(this);
}

void ThreadIndex::getAllIndex()
{
    GlobalVar::getData(indexData,1.8,QUrl("http://push2.eastmoney.com/api/qt/ulist.np/get?fid=f14&pi=0&pz=40&po=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&fields=f14,f12,f2,f3&np=1&secids=1.000001%2C0.399001%2C0.399006%2C100.HSI%2C100.N225%2C100.KS11%2C1.000688%2c100.TWII%2C100.SENSEX%2C100.DJIA%2C100.SPX%2C100.NDX%2C100.SX5E%2C100.GDAXI%2C100.RTS%2C100.FTSE%2C100.FCHI%2C100.AS51&_=1662857186403"));
    if (GlobalVar::timeOutFlag[1])
        GlobalVar::timeOutFlag[1]=false;
    else
        {
            GlobalVar::getData(futuresData,1.8,QUrl("http://futsseapi.eastmoney.com/list/block?orderBy=name&sort=desc&pageSize=20&pageIndex=0&blockName=financial&_=1666243575249"));
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
    m_mutex.lock();
    QJsonParseError *jsonError=new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(indexData, jsonError);

    if (jsonError->error == QJsonParseError::NoError)
    {
        GlobalVar::mIndexList.clear();
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();

            IndexInfo info;
            info.code = ceilMap.value("f12").toString();
            if (i>2 && i<16)
                info.code = "100."+ceilMap.value("f12").toString();
            info.name = ceilMap.value("f14").toString();
            info.close = ceilMap.value("f2").toString();
            info.pctChg=ceilMap.value("f3").toString();
            GlobalVar::mIndexList.append(info);
        }
    }
    m_mutex.unlock();
//    for (int i = 0; i < GlobalVar::mIndexList.size(); ++i)
//        qDebug()<<i<<GlobalVar::mIndexList.at(i).name<<GlobalVar::mIndexList.at(i).code;
}

void ThreadIndex::initFuturesList()
{
    m_mutex.lock();
    QJsonParseError *jsonError=new QJsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(futuresData, jsonError);

    if (jsonError->error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("list").toArray();
        for (int i = 0; i < data.size(); ++i)
        {
            QJsonValue value = data.at(i);
            QVariantMap ceilMap = value.toVariant().toMap();

            IndexInfo info;
            info.name = ceilMap.value("name").toString();
            info.close = ceilMap.value("p").toString();
            info.pctChg=ceilMap.value("zdf").toString();
            if (info.name=="A50期指当月连续" || info.name=="小型道指当月连续")
                GlobalVar::mIndexList.append(info);
        }

//        for (int i = 0; i < GlobalVar::mIndexList.size(); ++i)
//            qDebug()<<i<<GlobalVar::mIndexList.at(i).name<<GlobalVar::mIndexList.at(i).close<<GlobalVar::mIndexList.at(i).pctChg;
    }
    m_mutex.unlock();
}

