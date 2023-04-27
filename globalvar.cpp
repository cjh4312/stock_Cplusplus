
#include "globalvar.h"
#include <QCoreApplication>

GlobalVar::GlobalVar()
{
    if (isFirst)
    {
        isFirst=false;
        TableList<<"代码"<<"名称"<<"最新价"<<"涨跌幅"<<"换手率"<<"成交额"<<"涨速"<<"市盈率"<<"总市值"<<"流通市值"<<"今年"<<"60日"<<"成交量"<< "最高"<< "最低"<< "今开"<<"昨收";
        currentPath=QCoreApplication::applicationDirPath();
        settings=new QSettings(currentPath+"/config.ini",QSettings::IniFormat);
//        qDebug()<<currentPath;
        pRed.setColor(QPalette::WindowText, Qt::red);
        pGreen.setColor(QPalette::WindowText, QColor(0,191,0));
        pBlack.setColor(QPalette::WindowText, Qt::black);
        pBlue.setColor(QPalette::WindowText, Qt::blue);
    }
}

bool GlobalVar::isWorkDay()
{
    QString current_week = QDateTime::currentDateTime().toString("ddd");
//    qDebug()<<current_week;
    if (current_week!="Sat" and current_week!="Sun")
        return true;
    else
        return false;
}

bool GlobalVar::isZhMarketDay()
{
    QStringList vacation=GlobalVar::settings->value("Vacation_ZH").toStringList();
    QString cur_date=QDateTime::currentDateTime().toString("MMdd");
    int cur_time= QDateTime::currentDateTime().toString("hhmmss").toInt();
//    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay() && ((cur_time>=91500&&cur_time<=113010) || (cur_time>=130000&&cur_time<=150010)))
        return true;
    else
        return false;
}

bool GlobalVar::isUSMarketDay()
{
    QStringList vacation=GlobalVar::settings->value("Vacation_US").toStringList();
    QString cur_date=QDateTime::currentDateTime().addSecs(-46800).toString("MMdd");
    int cur_time= QDateTime::currentDateTime().addSecs(-46800).toString("hhmmss").toInt();
    //    qDebug()<<cur_time;
    if (not vacation.contains(cur_date) && isWorkDay() && (cur_time>=83010 and cur_time<=150010))
        return true;
    else
        return false;
}

bool GlobalVar::isHKMarketDay()
{
    QStringList vacation=GlobalVar::settings->value("Vacation_HK").toStringList();
    QString cur_date=QDateTime::currentDateTime().toString("MMdd");
    int cur_time= QDateTime::currentDateTime().toString("hhmmss").toInt();
    //    qDebug()<<cur_date<<vacation;
    if (not vacation.contains(cur_date) && isWorkDay() && ((cur_time>=90000&&cur_time<=121510) || (cur_time>=130000&&cur_time<=161510)))
        return true;
    else
        return false;
}

bool isNum(QString str)
{
    bool isNum;
    str.toDouble(&isNum);
    return isNum;
}

QString GlobalVar::getComCode()
{
//    qDebug()<<GlobalVar::curCode;
    if (curCode.length()==5 && isNum(curCode))
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

void GlobalVar::getEastData(QNetworkAccessManager *naManager, QByteArray &allData,float timeOut, const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    //    QNetworkAccessManager *naManager = new QNetworkAccessManager(this);
    //    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);
    QEventLoop loop;
    QNetworkReply *reply = naManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(timeOut*1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    if (reply->isFinished())
    {
        timer.stop();
        // 检测网页返回状态码，常见是200，404等，200为成功
        int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        //            qDebug() << "statusCode:" << statusCode;
        // 判断有没有错误
        if (reply->error())
            qDebug()<<reply->errorString();

        // 判断是否需要重定向
        else if (statusCode >= 200 && statusCode <300){
            // 准备读数据
            //        QTextCodec *codec = QTextCodec::codecForName("utf8");
            //        QString all = codec->toUnicode(reply->readAll());
            allData=reply->readAll();
            //          qDebug()<<allData;
        }
        else if (statusCode >=300 && statusCode <400){
            // 获取重定向信息
            const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            // 检测是否需要重定向，如果不需要则读数据
            if (!redirectionTarget.isNull()) {
                const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
                getEastData(naManager,allData,timeOut*1000,redirectedUrl);
                qDebug()<< "http redirect to " << redirectedUrl.toString();
            }
        }
    }
    else
    {
        //超时，未知状态
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QStringList s={"http://push2his.eastmoney.com/api/qt/stock/kline",
                        "http://push2.eastmoney.com/api/qt/ulist",
                        "http://futsseapi.eastmoney.com/list/block",
                        "https://www.jin10.com",
                        "http://push2.eastmoney.com/api/qt/clist",
                        "https://push2his.eastmoney.com/api/qt/stock/trends2",
                        "http://push2.eastmoney.com/api/qt/stock/details",
                        "http://push2.eastmoney.com/api/qt/stock",
                        "http://api.waditu.com"};
        QStringList n={"candle chart of thread",
                        "index of thread",
                        "future index of thread",
                        "newsreport of thread",
                        "table stock of thread",
                        "time share chart of thread",
                        "time share tick of thread",
                        "buy sell of thread",
                        "post"};
        for (int i=0;i<s.count();++i)
        {
            if (url.toString().contains(s[i]))
            {
                qDebug()<< "timeOut" <<QDateTime::currentDateTime().toString()<< n[i]<<timeOut;
                break;
            }
            if (i==7)
                qDebug()<< "timeOut" <<QDateTime::currentDateTime()<<url<<timeOut;
        }
//
    }
    if (reply!=nullptr)
    {
        reply->deleteLater();
        reply = nullptr;
    }
}

void GlobalVar::postData(QNetworkAccessManager *naManager, QByteArray &postArray, QByteArray &allData,float timeOut, const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QEventLoop loop;
    QNetworkReply *reply = naManager->post(request,postArray);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QTimer timer;
    timer.singleShot(timeOut*1000, &loop, SLOT(quit()));
    timer.start();
    loop.exec();

    if (reply->isFinished())
    {
        timer.stop();
        // 检测网页返回状态码，常见是200，404等，200为成功
        int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        //            qDebug() << "statusCode:" << statusCode;
        // 判断有没有错误
        if (reply->error())
            qDebug()<<reply->errorString();

        // 判断是否需要重定向
        else if (statusCode >= 200 && statusCode <300){
            // 准备读数据
            //        QTextCodec *codec = QTextCodec::codecForName("utf8");
            //        QString all = codec->toUnicode(reply->readAll());
            allData=reply->readAll();
//            qDebug()<<QString(allData);
        }
    }
    else
    {
        //超时，未知状态
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QStringList s={"http://push2his.eastmoney.com/api/qt/stock/kline",
                         "http://push2.eastmoney.com/api/qt/ulist",
                         "http://futsseapi.eastmoney.com/list/block",
                         "https://www.jin10.com",
                         "http://push2.eastmoney.com/api/qt/clist",
                         "https://push2his.eastmoney.com/api/qt/stock/trends2",
                         "http://push2.eastmoney.com/api/qt/stock/details",
                         "http://push2.eastmoney.com/api/qt/stock"};
        QStringList n={"candle chart of thread",
                         "index of thread",
                         "future index of thread",
                         "newsreport of thread",
                         "table stock of thread",
                         "time share chart of thread",
                         "time share tick of thread",
                         "buy sell of thread"};
        for (int i=0;i<s.count();++i)
        {
            if (url.toString().contains(s[i]))
            {
                qDebug()<< "timeOut" <<QDateTime::currentDateTime().toString()<< n[i]<<timeOut;
                break;
            }
            if (i==7)
                qDebug()<< "timeOut" <<QDateTime::currentDateTime()<<url<<timeOut;
        }
        //
    }
    if (reply!=nullptr)
    {
        reply->deleteLater();
        reply = nullptr;
    }
}

QString GlobalVar::format_conversion(float data)
{
    if (data>=100000000)
        return QString::number(data/100000000, 'f', 2)+"亿";
    else if (data>=10000)
        return QString::number(data/10000, 'f', 2)+"万";
    else
        return QString::number(data,'f',2);
}

QString GlobalVar::peelStr(const QString s,const QString begin,const QString end)
{
    int bPos=s.indexOf(begin)+begin.length();
    int ePos=s.lastIndexOf(end);
    return s.mid(bPos,ePos-bPos);
}

QPair<QString, QString> GlobalVar::cutStr(const QString s,const QString begin,const QString end)
{
    int bPos=s.indexOf(begin)+begin.length();
    int ePos=s.indexOf(end);
    QPair<QString, QString> pair;
    pair.first=s.mid(bPos,ePos-bPos);
    pair.second=s.mid(ePos+end.length(),-1);
    return pair;
}

QString GlobalVar::curCode="600519";
QString GlobalVar::curName="贵州茅台";
QStringList GlobalVar::TableList;
float GlobalVar::preClose=0.00;
int GlobalVar::WhichInterface = 1;
int GlobalVar::preInterface=1;
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
int GlobalVar::KRange=KRANGE;
int GlobalVar::offsetEnd=OFFSETEND;
int GlobalVar::offsetLocal=GlobalVar::KRange;
float GlobalVar::timeShareHighLowPoint[5]={0.0};
//float GlobalVar::candleHighLowPoint[5]={0.0};
float GlobalVar::buySellPrice[10]={0.0};
float GlobalVar::buySellNum[10]={0.0};
float GlobalVar::baseInfoData[13]={0.0};
int GlobalVar::curSortNum=3;
bool GlobalVar::is_asc=false;
QPalette GlobalVar::pRed;
QPalette GlobalVar::pGreen;
QPalette GlobalVar::pBlack;
QPalette GlobalVar::pBlue;
int GlobalVar::upNums=0;
int GlobalVar::downNums=0;
bool GlobalVar::isNewsReport=false;
bool GlobalVar::isSayNews=true;
bool GlobalVar::isFirst=true;
int GlobalVar::trendsTotal=0;
int GlobalVar::curBlock=0;
QString GlobalVar::currentPath;

