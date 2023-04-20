
#include "threadtable.h"
#include "globalvar.h"

ThreadTable::ThreadTable(QObject *parent)
    : QObject{parent}
{
    naManager = new QNetworkAccessManager(this);
}

void ThreadTable::getTableData()
{
    if (isRunning)
        return;
    isRunning=true;
    if (GlobalVar::WhichInterface==1)
    {
        //    QTime startTime=QTime::currentTime();
        GlobalVar::getEastData(naManager,allData,2,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=6000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&fid=f22&fs=m:0+t:6,m:0+t:80,m:1+t:2,m:1+t:23,m:0+t:81+s:2048&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667954879297"));
        initTableList();
        if (isFirstReadMyStock)
        {
            readMyStock();
            isFirstReadMyStock=false;
        }
        reFlaseMyStock();
        //    QTime endTime=QTime::currentTime();
        //    qDebug() << startTime.msecsTo(endTime);
    }
    else if (GlobalVar::WhichInterface==2)
    {
        GlobalVar::getEastData(naManager,allData,3,QUrl("http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=5000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs=m:116+t:3,m:116+t:4,m:116+t:1,m:116+t:2&fields=f2,f3,f5,f6,f8,f9,f12,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667966922156"));
        initTableList();
    }
    else if (GlobalVar::WhichInterface==5)
    {
        QString fs;
        if (GlobalVar::isUsZhStock)
            fs = "b:mk0201";
        else
            fs = "m:105,m:106,m:107";
        QString s="http://push2.eastmoney.com/api/qt/clist/get?pn=1&pz=20000&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&fid=f6&fs="+fs+"&fields=f2,f3,f5,f6,f8,f9,f12,f13,f14,f15,f16,f17,f18,f20,f21,f24,f25,f22&_=1667962034515";
        GlobalVar::getEastData(naManager,allData,3,QUrl(s));
        initTableList();
    }
    if (not GlobalVar::mTableList.empty())
        emit getTableDataFinished();
    isRunning=false;
}

void ThreadTable::initTableList()
{
    GlobalVar::mTableList.clear();
    GlobalVar::mTableListCopy.clear();
    GlobalVar::mRisingSpeedList.clear();
    GlobalVar::upNums=0;
    GlobalVar::downNums=0;
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

            StockInfo info;
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
            info.CirculatedValue=ceilMap.value("f21").toFloat();
            info.pctYear=ceilMap.value("f25").toFloat();
            info.pctSixty=ceilMap.value("f24").toFloat();
            info.volume = ceilMap.value("f5").toFloat();
            info.high = ceilMap.value("f15").toFloat();
            info.low = ceilMap.value("f16").toFloat();
            info.open=ceilMap.value("f17").toFloat();
            info.preClose=ceilMap.value("f18").toFloat();
            GlobalVar::mTableList.append(info);
            if (GlobalVar::WhichInterface!=2 and GlobalVar::WhichInterface!=5)
            {
                GlobalVar::mTableListCopy.append(info);
                if (i<=19)
                    GlobalVar::mRisingSpeedList.append(info);
            }
            //            mModel->setItem(i, 0, new QStandardItem(info.code));
            //            mModel->setItem(i, 1, new QStandardItem(info.name));
            //            mModel->setItem(i, 2, new QStandardItem(QString::number(info.close)));
            //            mModel->setItem(i, 3, new QStandardItem(QString::number(info.pctChg)));
            //            mModel->setItem(i, 4, new QStandardItem(QString::number(info.turn)));
            //            mModel->setItem(i, 5, new QStandardItem(QString::number(info.amount)));
        }
        if (GlobalVar::WhichInterface!=2 and GlobalVar::WhichInterface!=5)
            GlobalVar::sortByColumn(&GlobalVar::mTableListCopy,0,true);
        GlobalVar::sortByColumn(&GlobalVar::mTableList,GlobalVar::curSortNum,GlobalVar::is_asc);
    }
}

void ThreadTable::readMyStock()
{
    QFile file("e:/cjh/Documents/qt/oneStock/list/my_stock.csv");

    if (file.open(QIODevice::ReadOnly))
    {
//        QTextStream read = QTextStream(&file);
        QTextCodec *codec = QTextCodec::codecForName("GBK");

//        QStringList Data = read.readAll().split("\n",Qt::SkipEmptyParts);
        QStringList Data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
        for(int i=1;i<Data.count();++i)
        {
            QStringList strLine = Data.at(i).split(",");     //一行中的单元格以，区分
            StockInfo info;
            info.code=strLine.at(0);
//            info.name=strLine.at(1);
//            info.close=strLine.at(2).toFloat();
//            info.pctChg=strLine.at(3).toFloat();
//            info.turn=strLine.at(4).toFloat();
//            info.amount=strLine.at(5).toFloat();
//            info.velocity=strLine.at(6).toFloat();
//            info.pe=strLine.at(7).toFloat();
//            info.totalValue=strLine.at(8).toFloat();
//            info.CirculatedValue=strLine.at(9).toFloat();
//            info.pctYear=strLine.at(10).toFloat();
//            info.pctSixty=strLine.at(11).toFloat();
//            info.volume=strLine.at(12).toFloat();
//            info.high=strLine.at(13).toFloat();
//            info.low=strLine.at(14).toFloat();
//            info.open=strLine.at(15).toFloat();
//            info.preClose=strLine.at(16).toFloat();
            GlobalVar::mMyStockList.append(info);
        }
        file.close();
    }
}

void ThreadTable::reFlaseMyStock()
{
    int n=GlobalVar::mTableListCopy.count();
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

