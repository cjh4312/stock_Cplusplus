
#include "threadindex.h"
#include "globalvar.h"

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
//    QTime t=QDateTime::currentDateTime().time();
    GlobalVar::getData(indexData,3,QUrl("https://push2.eastmoney.com/api/qt/ulist.np/get?&pz=20&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&fields=f14,f2,f3&secids=1.000001%2C0.399001%2C0.399006%2C100.HSI%2C100.N225%2C100.KS11%2C1.000688%2c100.TWII%2C100.SENSEX%2C100.DJIA%2C100.SPX%2C100.NDX%2C100.SX5E%2C100.GDAXI%2C100.RTS%2C100.FTSE%2C100.FCHI%2C100.AS51%2C103.YM00Y%2C104.CN00Y&_=1662857186403"));
    if (GlobalVar::timeOutFlag[1])
        GlobalVar::timeOutFlag[1]=false;
    else
        {
            initIndexList();
            emit getIndexFinished();
        }
//    qDebug()<<t.msecsTo(QDateTime::currentDateTime().time());
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

