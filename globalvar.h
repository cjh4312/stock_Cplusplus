
#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>
#include "stockinfo.h"
#include <QPalette>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>

class GlobalVar
{
public:
    GlobalVar();
    static bool isWorkDay(QDateTime curTime);
    static bool isZhWorkDay(QDateTime curTime,bool select);
    static QDateTime curRecentWorkDay(bool select);
    static bool isZhMarketDay(QDateTime curTime);
    static bool isUSMarketDay(QDateTime curTime);
    static bool isHKMarketDay(QDateTime curTime);
    static QString getComCode();
    static QString getStockSymbol();
    static void sortByColumn(QList<StockInfo> *mList,int column,bool is_asc);
    static void getData(QByteArray &allData,float timeOut,QNetworkRequest requests);
    static void getData(QByteArray &allData,float timeOut,const QUrl &url);
    static void postData(const QByteArray &postArray,QByteArray &allData, float timeOut, const QUrl &url);
    static QString format_conversion(float data);
    static QString peelStr(const QString &s,QString begin,QString end);
    static QPair<QString, QString> cutStr(const QString &s,QString begin,QString end);
    static QString getContent(const QString &s);
    static void getAllContent(QString &content,QStringList &strList,QString begin);
    static bool isInt(QString s);

    static QString curCode; //当前代码
    static bool isBoard;   //判断是否板块
    static QString curBoard; //当前板块
    static QString curName;  //当前股票名称
    static QString EPSReportDate;  //EPS报告期
//    static QStringList tableHeader;
    static float preClose; //昨日收盘价
    static int WhichInterface;  //界面标识
    static bool isKState;     //标识k线界面
    static bool isUsZhStock;  //是否是中概股
    static QString circle_green_SheetStyle;  //交易时段标识
    static QString circle_red_SheetStyle;
    static QSettings *settings;    //设置ini文件
    static QList<StockInfo> mTableList;    //所有股票
    static QList<StockInfo> mTableListCopy;  //复制所有股票 按代码排序
    static QList<StockInfo> mRisingSpeedList;  //涨速
    static QList<StockInfo> mMyStockList;     //自选股
    static QList<IndexInfo> mIndexList;      //全球指数
    static QList<timeShareTickInfo> mTimeShareTickList;   //每笔成交
    static QList<timeShartChartInfo> mTimeShareChartList;  //分时图
    static QList<candleChartInfo> mCandleChartList;      //k线图
//    static QList<QStringList> mFundFlowList;         //资金流
//    static bool timeOutFlag[10];                 //超时标志
    static int KRange;                  //k线范围
    static float timeShareHighLowPoint[5];     //记录分时价格高低点 成交量高低点 涨跌停
//    static float candleHighLowPoint[5];
    static float buySellPrice[10];             //买卖价格1-5
    static float buySellNum[10];              //买卖数量1-5
    static float baseInfoData[13];            //基本信息
    static int curSortNum;                   //当前排序列
    static bool is_asc;                     //标识升序和倒序
    static QPalette pRed;                   //定义颜色
    static QPalette pGreen;
    static QPalette pBlack;
    static QPalette pBlue;
    static int upNums;                     //上涨家数
    static int downNums;                   //下跌家数
    static int trendsTotal;                //分时图交易数据量
    static bool isSayNews;                 //是否语音播报
    static int curBlock;                   //记录查询股票在哪一行
    static QString currentPath;            //当前路径
    static int offsetEnd;                  //最近k线是否显示的偏移量
    static int offsetLocal;                //k线显示范围的偏移量
};

#endif // GLOBALVAR_H
