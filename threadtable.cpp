
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
    // QTime t=QDateTime::currentDateTime().time();
    QString fs="";
    if (GlobalVar::WhichInterface==1)
    {
        fs="m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048";
        //"f37,f38,f39,f40,f41,f45,f46,f48,f49,f57,f100,f102,f112,f113,f129,f135";
        QString s="https://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs="+fs+"&fields=f2,f3,f5,f6,f7,f8,f9,f10,f11,f12,f14,f15,f16,f17,f18,f20,f21,f22,f23,f24,f25,f31,f32&_=1667954879297";
        GlobalVar::getData(allData,3,QUrl(s));

        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
            {
                initTableList();
                reFlaseMyStock();
                emit getTableDataFinished();
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
        GlobalVar::getData(allData,3.5,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs="+fs+"&fields=f2,f3,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f31,f32&_=1667966922156"));
        if (GlobalVar::timeOutFlag[5])
            GlobalVar::timeOutFlag[5]=false;
        else
        {
            initTableList();
            emit getTableDataFinished();
        }
    }
    // qDebug()<<t.msecsTo(QDateTime::currentDateTime().time());
}

void ThreadTable::initTableList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(allData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        for (int i=0;i<4;++i)
        {
            GlobalVar::upNums[i]=0;
            GlobalVar::downNums[i]=0;
        }
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("diff").toArray();
        QJsonValue value;
        QVariantMap ceilMap;
        StockInfo info;
        if (GlobalVar::WhichInterface==1)
        {
            QList<StockInfo> tempTableListCopy;
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
                info.buy1=ceilMap.value("f31").toFloat();
                info.sell1=ceilMap.value("f32").toFloat();
                info.high = ceilMap.value("f15").toFloat();
                info.low = ceilMap.value("f16").toFloat();
                info.preClose=ceilMap.value("f18").toFloat();
                info.turn=ceilMap.value("f8").toFloat();
                info.amount=ceilMap.value("f6").toFloat();
                info.velocity = ceilMap.value("f22").toFloat();
                info.pe = ceilMap.value("f9").toFloat();
                info.totalValue = ceilMap.value("f20").toFloat();
                info.circulatedValue=ceilMap.value("f21").toFloat();
                info.pctYear=ceilMap.value("f25").toFloat();
                info.pctSixty=ceilMap.value("f24").toFloat();
                info.volume = ceilMap.value("f5").toFloat();
                info.open=ceilMap.value("f17").toFloat();
                info.amplitude=ceilMap.value("f7").toFloat();
                info.qrr=ceilMap.value("f10").toFloat();
                info.pbr=ceilMap.value("f23").toFloat();
                if (info.pctChg>=8)
                {
                    GlobalVar::upNums[3]+=1;
                    GlobalVar::upNums[2]+=1;
                    GlobalVar::upNums[0]+=1;
                    // int ph=110;
                    // float a=int(info.preClose*ph+0.5)/100.0;
                    // if (info.code.left(1)=="3" or info.code.left(3)=="688")
                    // {
                    //     ph=120;
                    //     a=int(info.preClose*ph+0.5)/100.0;
                    // }
                    // else if(info.code.left(1)=="4" or info.code.left(1)=="8")
                    // {
                    //     ph=130;
                    //     a=int(info.preClose*ph)/100.0;
                    // }
                    if (/*info.close==a and */info.sell1==0)
                        GlobalVar::upNums[1]+=1;
                }
                else if (info.pctChg>=5)
                {
                    GlobalVar::upNums[2]+=1;
                    GlobalVar::upNums[0]+=1;

                }
                else if (info.pctChg>0)
                    GlobalVar::upNums[0]+=1;
                else if (info.pctChg<=-8)
                {
                    // GlobalVar::downNums[3]+=1;
                    // GlobalVar::downNums[2]+=1;
                    // GlobalVar::downNums[0]+=1;
                    // int pl=90;
                    // float b=int(info.preClose*pl+0.5)/100.0;
                    // if (info.code.left(1)=="3" or info.code.left(3)=="688")
                    // {
                    //     pl=80;
                    //     b=int(info.preClose*pl+0.5)/100.0;
                    // }
                    // else if(info.code.left(1)=="4" or info.code.left(1)=="8")
                    // {
                    //     pl=70;
                    //     b=int(info.preClose*pl)/100.0;
                    // }
                    if (/*info.close==b and */info.buy1==0)
                        GlobalVar::downNums[1]+=1;
                }
                else if (info.pctChg<=-5)
                {
                    GlobalVar::downNums[2]+=1;
                    GlobalVar::downNums[0]+=1;
                }
                else if (info.pctChg<0)
                    GlobalVar::downNums[0]+=1;
                tempTableListCopy.append(info);
            }
            GlobalVar::mTableListCopy=tempTableListCopy;
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
                if (info.pctChg>=8)
                {
                    GlobalVar::upNums[3]+=1;
                    GlobalVar::upNums[2]+=1;
                    GlobalVar::upNums[0]+=1;
                }
                else if (info.pctChg>=5)
                {
                    GlobalVar::upNums[2]+=1;
                    GlobalVar::upNums[0]+=1;
                }
                else if (info.pctChg>0)
                    GlobalVar::upNums[0]+=1;
                else if (info.pctChg<=-8)
                {
                    GlobalVar::downNums[3]+=1;
                    GlobalVar::downNums[2]+=1;
                    GlobalVar::downNums[0]+=1;
                }
                else if (info.pctChg<=-5)
                {
                    GlobalVar::downNums[2]+=1;
                    GlobalVar::downNums[0]+=1;
                }
                else if (info.pctChg<0)
                    GlobalVar::downNums[0]+=1;
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
                info.amplitude=ceilMap.value("f7").toFloat();
                info.qrr=ceilMap.value("f10").toFloat();
                info.pbr=ceilMap.value("f23").toFloat();
                info.buy1=ceilMap.value("f31").toFloat();
                info.sell1=ceilMap.value("f32").toFloat();
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

