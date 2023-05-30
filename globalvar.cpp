
#include "globalvar.h"
#include <QCoreApplication>

GlobalVar::GlobalVar()
{

}

bool GlobalVar::isWorkDay(QDateTime curTime)
{
    QString current_week = curTime.toString("ddd");
//    qDebug()<<current_week;
    if (current_week!="Sat" and current_week!="Sun")
        return true;
    else
        return false;
}

bool GlobalVar::isZhWorkDay(QDateTime curTime,int select)
{
    QStringList vacation=GlobalVar::settings->value("Vacation_ZH").toStringList();
    QDateTime local=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();
    int time=150000;
    if (select==1)
        time=163000;
    else if(select==2)
        time=230000;
    if (not vacation.contains(cur_date) && isWorkDay(curTime))
    {
        if (local==curTime && cur_time<time)
            return false;
        return true;
    }
    else
        return false;
}

QDateTime GlobalVar::curRecentWorkDay(int select)
{
    QDateTime curTime=QDateTime::currentDateTime();
    for (int i=0;i<15;++i)
        if (isZhWorkDay(curTime.addDays(-i),select))
            return curTime.addDays(-i);
    return curTime;
}

bool GlobalVar::isZhMarketDay(QDateTime curTime)
{
    QStringList vacation=GlobalVar::settings->value("Vacation_ZH").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();
//    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && ((cur_time>=91500&&cur_time<=113010) || (cur_time>=130000&&cur_time<=150010)))
        return true;
    else
        return false;
}

bool GlobalVar::isUSMarketDay(QDateTime curTime)
{
    QStringList vacation=GlobalVar::settings->value("Vacation_US").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.addSecs(-46800).toString("MMdd");
    int cur_time= curTime.addSecs(-46800).toString("hhmmss").toInt();
    //    qDebug()<<cur_time;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && (cur_time>=83010 and cur_time<=150010))
        return true;
    else
        return false;
}

bool GlobalVar::isHKMarketDay(QDateTime curTime)
{
    QStringList vacation=GlobalVar::settings->value("Vacation_HK").toStringList();
//    QDateTime curTime=QDateTime::currentDateTime();
    QString cur_date=curTime.toString("MMdd");
    int cur_time= curTime.toString("hhmmss").toInt();
    //    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay(curTime) && ((cur_time>=90000&&cur_time<=121510) || (cur_time>=130000&&cur_time<=161510)))
        return true;
    else
        return false;
}

bool GlobalVar::isInt(QString s)
{
    bool isInt;
    s.toInt(&isInt);
    return isInt;
}

QString GlobalVar::getComCode()
{
//    qDebug()<<GlobalVar::curCode;
    if (curCode.length()==5 && isInt(curCode))
        return "116."+curCode;
    else if (curCode.left(1)=="1")
        return curCode;
    else
    {
        if (GlobalVar::curCode.left(1)=="6")
            return "1."+curCode;
        else
            return "0."+curCode;
    }
}

QString GlobalVar::getStockSymbol()
{
    QString symbol;
    if (GlobalVar::curCode.left(1)=="6")
        symbol="SH"+GlobalVar::curCode;
    else if (GlobalVar::curCode.left(1)=="8" or GlobalVar::curCode.left(1)=="4")
        symbol="BJ"+GlobalVar::curCode;
    else if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="0")
        symbol="SZ"+GlobalVar::curCode;
    return symbol;
}

void GlobalVar::sortByColumn(QList<StockInfo> *mList, const int column, const bool is_asc)
{
    std::sort(mList->begin(), mList->end(),
              [column,is_asc](const StockInfo &infoA,const StockInfo &infoB){
                  switch(column)
                  {
                  case 0:
                      return is_asc?(infoA.code<infoB.code):(infoA.code>infoB.code);
                  case 1:
                      return is_asc?(infoA.name<infoB.name):(infoA.name>infoB.name);
                  case 2:
                      return is_asc?(infoA.close<infoB.close):(infoA.close>infoB.close);
                  case 3:
                      return is_asc?(infoA.pctChg<infoB.pctChg):(infoA.pctChg>infoB.pctChg);
                  case 4:
                      return is_asc?(infoA.turn<infoB.turn):(infoA.turn>infoB.turn);
                  case 5:
                      return is_asc?(infoA.amount<infoB.amount):(infoA.amount>infoB.amount);
                  case 6:
                      return is_asc?(infoA.velocity<infoB.velocity):(infoA.velocity>infoB.velocity);
                  case 7:
                      return is_asc?(infoA.pe<infoB.pe):(infoA.pe>infoB.pe);
                  case 8:
                      return is_asc?(infoA.totalValue<infoB.totalValue):(infoA.totalValue>infoB.totalValue);
                  case 9:
                      return is_asc?(infoA.CirculatedValue<infoB.CirculatedValue):(infoA.CirculatedValue>infoB.CirculatedValue);
                  case 10:
                      return is_asc?(infoA.pctYear<infoB.pctYear):(infoA.pctYear>infoB.pctYear);
                  case 11:
                      return is_asc?(infoA.pctSixty<infoB.pctSixty):(infoA.pctSixty>infoB.pctSixty);
                  case 12:
                      return is_asc?(infoA.volume<infoB.volume):(infoA.volume>infoB.volume);
                  case 13:
                      return is_asc?(infoA.high<infoB.high):(infoA.high>infoB.high);
                  case 14:
                      return is_asc?(infoA.low<infoB.low):(infoA.low>infoB.low);
                  case 15:
                      return is_asc?(infoA.open<infoB.open):(infoA.open>infoB.open);
                  case 16:
                      return is_asc?(infoA.preClose<infoB.preClose):(infoA.preClose>infoB.preClose);
                  default:
                      return is_asc?(infoA.pctChg<infoB.pctChg):(infoA.pctChg>infoB.pctChg);
                  }
              });
}

void GlobalVar::getData(QByteArray &allData,float timeOut, const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkAccessManager naManager =QNetworkAccessManager();
    QEventLoop loop;
    QNetworkReply *reply = naManager.get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(timeOut*1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 200)
    {
        timer.stop();
        allData=reply->readAll();
    }
    else
    {
        //超时，未知状态
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QStringList s={"http://push2his.eastmoney.com/api/qt/stock/kline",
                         "http://push2.eastmoney.com/api/qt/ulist",
                         "http://futsseapi.eastmoney.com/list/block",
                         "https://www.jin10.com",
                         "https://finance.eastmoney.com",
                         "http://push2.eastmoney.com/api/qt/clist",
                         "https://push2his.eastmoney.com/api/qt/stock/trends2",
                         "http://push2.eastmoney.com/api/qt/stock/details",
                         "http://push2.eastmoney.com/api/qt/stock",
                         "https://datacenter-web.eastmoney.com"};
        QStringList n={"candle chart of thread",
                         "index of thread",
                         "future index of thread",
                         "jin10 newsreport of thread",
                         "east newsreport of thread",
                         "table stock of thread",
                         "time share chart of thread",
                         "time share tick of thread",
                         "buy sell of thread",
                         "EPS of stock"};
        for (int i=0;i<s.count();++i)
        {
            if (url.toString().contains(s[i]))
            {
                //                timeOutFlag[i]=true;
//                QString s=QDateTime::currentDateTime().toString()+n[i];
                qDebug()<<statusCode<< reply->errorString() <<QDateTime::currentDateTime().toString()<< n[i]<<timeOut;

                break;
            }
            if (i==s.count()-1)
                qDebug()<<statusCode<< reply->errorString() <<QDateTime::currentDateTime()<<url<<timeOut;
        }
    }
    if (reply!=nullptr)
    {
        reply->deleteLater();
        reply = nullptr;
    }
}

void GlobalVar::getData(QByteArray &allData,float timeOut,QNetworkRequest request)
{
    QNetworkAccessManager naManager =QNetworkAccessManager();
    QEventLoop loop;
    QNetworkReply *reply = naManager.get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(timeOut*1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 200)
    {
        timer.stop();
        allData=reply->readAll();
    }
    else
    {
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        qDebug()<<statusCode<< reply->errorString() <<QDateTime::currentDateTime()<<request.url()<<timeOut;
    }

    if (reply!=nullptr)
    {
        reply->deleteLater();
        reply = nullptr;
    }
}

void GlobalVar::postData(const QByteArray &postArray,QByteArray &allData,float timeOut, const QUrl &url)
{
    QNetworkRequest request;
    QNetworkAccessManager naManager=QNetworkAccessManager();
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QEventLoop loop;
    QNetworkReply *reply = naManager.post(request,postArray);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(timeOut*1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 200)
    {
        timer.stop();
        allData=reply->readAll();
    }
    else
    {
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        qDebug()<<statusCode<< reply->errorString() <<QDateTime::currentDateTime()<<url<<timeOut;
    }

    if (reply!=nullptr)
    {
        reply->deleteLater();
        reply = nullptr;
    }
}

QString GlobalVar::format_conversion(float data)
{
    if (data>=100000000 or data<=-100000000)
        return QString::number(data/100000000, 'f', 2)+"亿";
    else if (data>=10000 or data<=-10000)
        return QString::number(data/10000, 'f', 2)+"万";
    else
        return QString::number(data,'f',2);
}

QString GlobalVar::peelStr(const QString &s,QString begin,QString end)
{
    int bPos=s.indexOf(begin)+begin.length();
    int ePos;
    if (end=="-1")
        ePos=end.toInt();
    else
        ePos=s.indexOf(end)+end.length();
    return s.mid(bPos,ePos);
}

QString GlobalVar::getContent(const QString &s)
{
    int bPos=s.indexOf(">")+1;
    int ePos=s.indexOf("<",bPos);
    return s.mid(bPos,ePos-bPos);
}

void GlobalVar::getAllContent(QString &content, QStringList &strList, QString begin)
{
    while(1)
    {
        if (content.indexOf(begin)==-1)
            break;
        content=GlobalVar::peelStr(content,begin,"-1");
        int bPos=content.indexOf(">")+1;
        int ePos=content.indexOf("<");
        if (ePos<=bPos)
        {
            QString s=content.mid(ePos+1,1);
            QString str=cutStr(content,"<"+s,"</"+s).first;
            getAllContent(str,strList,"<");
        }
        else
            strList.append(content.mid(bPos,ePos-bPos));
    }
}

QPair<QString, QString> GlobalVar::cutStr(const QString &s,QString begin,QString end)
{
    int bPos=s.indexOf(begin);
    int ePos=s.indexOf(end,bPos+1)+end.length();
    QPair<QString, QString> pair;
    if (ePos<=bPos)
    {
        QString str=peelStr(s,end,"-1");
        pair=cutStr(str,begin,end);
    }
    else
    {
        pair.first=s.mid(bPos,ePos-bPos);
        if (begin==end)
            pair.second=s.mid(ePos-end.length(),-1);
        else
            pair.second=s.mid(ePos,-1);
    }
    return pair;
}

QString GlobalVar::curCode="600519";
QString GlobalVar::curName="贵州茅台";
bool GlobalVar::isBoard=false;
QString GlobalVar::curBoard;
QString GlobalVar::EPSReportDate="每股收益";
QString GlobalVar::PEName="市盈率";
//QStringList GlobalVar::tableHeader;
float GlobalVar::preClose=0.00;
int GlobalVar::WhichInterface = 1;
bool GlobalVar::isKState=false;
bool GlobalVar::isUsZhStock=false;
QString GlobalVar::circle_green_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:green";
QString GlobalVar::circle_red_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid black;background:red";
QSettings *GlobalVar::settings;
QList<StockInfo> GlobalVar::mTableList;
QList<StockInfo> GlobalVar::mTableListCopy;
QList<IndexInfo> GlobalVar::mIndexList;
QList<StockInfo> GlobalVar::mRisingSpeedList;
QList<StockInfo> GlobalVar::mMyStockList;
QList<timeShareTickInfo> GlobalVar::mTimeShareTickList;
QList<timeShartChartInfo> GlobalVar::mTimeShareChartList;
QList<candleChartInfo> GlobalVar::mCandleChartList;
//QList<QStringList> GlobalVar::mFundFlowList;
//bool GlobalVar::timeOutFlag[10]={false};
int GlobalVar::KRange=KRANGE;
int GlobalVar::offsetEnd;
int GlobalVar::offsetLocal=GlobalVar::KRange;
float GlobalVar::timeShareHighLowPoint[5]={0.0};
//float GlobalVar::candleHighLowPoint[5]={0.0};
float GlobalVar::buySellPrice[10]={0.0};
float GlobalVar::buySellNum[10]={0.0};
float GlobalVar::baseInfoData[14]={0.0};
int GlobalVar::curSortNum=3;
bool GlobalVar::is_asc=false;
QPalette GlobalVar::pRed;
QPalette GlobalVar::pGreen;
QPalette GlobalVar::pBlack;
QPalette GlobalVar::pBlue;
int GlobalVar::upNums=0;
int GlobalVar::downNums=0;
bool GlobalVar::isSayNews=true;
int GlobalVar::trendsTotal=0;
int GlobalVar::curBlock=0;
QString GlobalVar::currentPath;
bool GlobalVar::areaFlag[5]={true};
QString GlobalVar::formulaContent;
int GlobalVar::mTableListNum;
QString GlobalVar::mCandleListCode;
QList<QStringList> GlobalVar::formula;
