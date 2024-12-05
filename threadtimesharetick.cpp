
#include "threadtimesharetick.h"

ThreadTimeShareTick::ThreadTimeShareTick(QObject *parent)
    : QObject{parent}
{
    QFile file(GlobalVar::currentPath+"/list/stock_list.csv");
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QStringList data=codec->toUnicode(file.readAll()).split("\n",Qt::SkipEmptyParts);
        for (int i=0;i<data.count();++i)
        {
            QStringList temp= data.at(i).split(",");
            areaData.append(temp);
        }
        std::sort(areaData.begin(),areaData.end(),[](QStringList a,QStringList b){
            return a[1]<b[1];
        });
    }
    file.close();
}

void ThreadTimeShareTick::getBuySellTimeShareTick(bool reset)
{
    QString t=GlobalVar::curCode.left(3);
    QList<QString> list;
    list<<"100"<<"122"<<"133"<<"103"<<"104";
    if (list.contains(t))
    {
        if (preCode==GlobalVar::curCode)
            return;
        QString url="http://push2.eastmoney.com/api/qt/stock/sse?mpi=2000&ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=1&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f58,f60,f62,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+GlobalVar::getComCode()+"&_=1666089246963";
        getSSEData(1,url);
        url="http://push2.eastmoney.com/api/qt/stock/details/sse?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=1&pos=-0&secid="+GlobalVar::getComCode();
        GlobalVar::mTimeShareTickList.clear();
        getSSEData(2,url);
        preCode=GlobalVar::curCode;
    }
    else
    {
        GlobalVar::getData(buySellData,0.5f,QUrl("http://push2.eastmoney.com/api/qt/stock/get?ut=fa5fd1943c7b386f172d6893dbfba10b&fltt=2&invt=1&volt=2&fields=f43,f44,f45,f46,f47,f48,f55,f57,f58,f60,f62,f164,f167,f168,f170,f116,f84,f85,f162,f31,f32,f33,f34,f35,f36,f37,f38,f39,f40,f20,f19,f18,f17,f16,f15,f14,f13,f12,f11,f531&secid="+GlobalVar::getComCode()+"&_=1666089246963"));
        if (GlobalVar::timeOutFlag[8])
            GlobalVar::timeOutFlag[8]=false;
        else
        {
            initBuySellList();
            findStockArea();
            emit getBuySellFinished();
        }
        QString pos="-0";
        if (preCode==GlobalVar::curCode and not reset and GlobalVar::WhichInterface==1 and not GlobalVar::mTimeShareTickList.isEmpty())
             pos="-10";
        float timeout=0.5;
        if (GlobalVar::WhichInterface==5)
            timeout=1.5;
        GlobalVar::getData(timeShareTickData,timeout,QUrl("http://push2.eastmoney.com/api/qt/stock/details/get?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos="+pos+"&secid="+GlobalVar::getComCode()));
        if (GlobalVar::timeOutFlag[7])
            GlobalVar::timeOutFlag[7]=false;
        else
        {
            initTimeShareTickList(pos);
            emit getTimeShareTickFinished();
        }
        preCode=GlobalVar::curCode;
//        if (preCode==GlobalVar::curCode)
//            return;
//        preCode=GlobalVar::curCode;
//        QString url="http://push2.eastmoney.com/api/qt/stock/details/sse?fields1=f1,f2,f3,f4&fields2=f51,f52,f53,f54,f55&mpi=2000&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&pos=-0&secid="+GlobalVar::getComCode();
//        GlobalVar::mTimeShareTickList.clear();
//        getSSEData(2,url);
    }
}

void ThreadTimeShareTick::getSSEData(int nums,QString url)
{
    QByteArray* qByteArray=new QByteArray();
    QString preCode=GlobalVar::curCode;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36");
    request.setUrl(QUrl(url));
    QNetworkAccessManager *naManager =new QNetworkAccessManager();
    QNetworkReply *reply= naManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=](){
        disconnect(reply);
        reply->deleteLater();
        delete qByteArray;
        naManager->deleteLater();
    });
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // qDebug()<<reply->error();
        if (GlobalVar::curCode!=preCode)
            reply->abort();
        else if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray tempData=reply->readAll();
            if (tempData.contains("data:"))
            {
                if (tempData.contains("\"data\":{\""))
                {
                    if (tempData.mid(tempData.size()-2,2)=="\n\n")
                    {
                        if (nums==1)
                        {
                            buySellData=tempData.mid(6,tempData.size()-8);
                            initBuySellList();
                            QString l=GlobalVar::curCode.left(1);
                            if (l=="3" or l=="6" or l=="0")
                                findStockArea();
                            emit getBuySellFinished();
                        }
                        else
                        {
                            timeShareTickData=tempData.mid(6,tempData.size()-8);
                            initSSETimeShareTickList();
                            emit getTimeShareTickFinished();
                        }
                    }
                    else
                        qByteArray->append(tempData);
                }
            }
            else
            {
                qByteArray->append(tempData);
                QByteArray tempByteArray=qByteArray->data();
                //                qDebug()<<tempByteArray;
                if (tempByteArray.mid(tempByteArray.size()-2,2)=="\n\n")
                {
                    if (nums==1)
                    {
                        buySellData=tempByteArray.mid(6,tempByteArray.size()-8);
                        initBuySellList();
                        QString l=GlobalVar::curCode.left(1);
                        if (l=="3" or l=="6" or l=="0")
                            findStockArea();
                        emit getBuySellFinished();
                    }
                    else
                    {
                        timeShareTickData=tempByteArray.mid(6,tempByteArray.size()-8);
                        initSSETimeShareTickList();
                        emit getTimeShareTickFinished();
                    }
                    qByteArray->clear();
                }
            }
        }
    });
}

void ThreadTimeShareTick::initBuySellList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(buySellData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        if (buySellData.contains("f57"))
        {

            QString code=jsonObject.value("data").toObject().value("f57").toString();
            QString str=GlobalVar::curCode;
            if (GlobalVar::curCode.contains("."))
                str=GlobalVar::curCode.split(".")[1];
            if (code!=str)
                return;
        }
        if (buySellData.contains("f60"))
            GlobalVar::preClose=jsonObject.value("data").toObject().value("f60").toDouble();
        if (buySellData.contains("f58"))
            GlobalVar::curName=jsonObject.value("data").toObject().value("f58").toString();
        for (int i=0;i<10;++i)
        {
            if (buySellData.contains(price[i].toUtf8()))
                GlobalVar::buySellPrice[i]=jsonObject.value("data").toObject().value(price[i]).toDouble();
            if (buySellData.contains(nums[i].toUtf8()))
                GlobalVar::buySellNum[i]=int(jsonObject.value("data").toObject().value(nums[i]).toDouble()+0.5);
            }
        for (int i=0;i<14;++i)
        {
            if (buySellData.contains(baseInfo[i].toUtf8()))
                GlobalVar::baseInfoData[i]=jsonObject.value("data").toObject().value(baseInfo[i]).toDouble();
        }
        if ((GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5) and
            buySellData.contains("f164"))
            GlobalVar::baseInfoData[13]=jsonObject.value("data").toObject().value("f164").toDouble();
        // if (buySellData.contains("f55"))
        //     GlobalVar::baseInfoData[12]=jsonObject.value("data").toObject().value("f55").toDouble();
        GlobalVar::EPSReportDate="每股收益";
        GlobalVar::PEName="市盈率";
        if (buySellData.contains("f62") and GlobalVar::WhichInterface==1)
        {
            int num=jsonObject.value("data").toObject().value("f62").toInt();
            if (num>0)
            {
                QString n[4]={"一","二","三","四"};
                GlobalVar::EPSReportDate="收益("+n[num-1]+")";
                GlobalVar::PEName="PE(动)";
            }
        }
    }
}

void ThreadTimeShareTick::initTimeShareTickList(QString pos)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(timeShareTickData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QString code=jsonObject.value("data").toObject().value("code").toString();
        QString str=GlobalVar::curCode;
        if (GlobalVar::curCode.contains("."))
            str=GlobalVar::curCode.split(".")[1];
        if (code!=str)
            return;
        GlobalVar::preClose=jsonObject.value("data").toObject().value("prePrice").toDouble();
        QJsonArray data=jsonObject.value("data").toObject().value("details").toArray();
        int s=data.size();
        timeShareTickInfo info;
        QStringList list;
        if (pos=="-10")
            {
                QString t=GlobalVar::mTimeShareTickList.at(GlobalVar::mTimeShareTickList.size()-1).time;
                for (int i = 0; i < s; ++i)
                {
                    list=data.at(i).toString().split(",");
                    if (list[0]<=t)
                    {
                        if (i==s-1)
                            return;
                    }
                    else
                    {
                        info.time=list[0];
                        info.price=list[1].toFloat();
                        info.nums=list[2].toInt();
                        info.direct=list[4].toInt();
                        info.tick=list[3].toInt();
                        GlobalVar::mTimeShareTickList.append(info);
                    }
                }
            }
        else
        {
            QList<timeShareTickInfo> mTimeShareTickList;
            for (int i = 0; i < s; ++i)
            {
                list=data.at(i).toString().split(",");
                info.time=list[0];
                info.price=list[1].toFloat();
                info.nums=list[2].toInt();
                info.direct=list[4].toInt();
                info.tick=list[3].toInt();
                mTimeShareTickList.append(info);
            }
            GlobalVar::mTimeShareTickList=mTimeShareTickList;
        }
    }
}

void ThreadTimeShareTick::initSSETimeShareTickList()
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(timeShareTickData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObject = doc.object();
        QJsonArray data=jsonObject.value("data").toObject().value("details").toArray();
        timeShareTickInfo info;
        QStringList list;
        for (int i = 0; i < data.size(); ++i)
        {
            list=data.at(i).toString().split(",");
            info.time=list[0];
            info.price=list[1].toFloat();
            info.nums=list[2].toInt();
            info.direct=list[4].toInt();
            info.tick=list[3].toInt();
            GlobalVar::mTimeShareTickList.append(info);
        }
    }
}

void ThreadTimeShareTick::findStockArea()
{
    int l = 0;
    int r = areaData.count() - 1;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        QStringList s=areaData.at(mid);
        if(s[1]==GlobalVar::curCode)
        {
            if (s[3]!="")
                GlobalVar::curName=s[2]+"("+s[3]+")"+"--"+s[4];
            break;
        }
        else if (s[1]>GlobalVar::curCode)
            r = mid - 1;
        else
            l = mid + 1;
    }
}

