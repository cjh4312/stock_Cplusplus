
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
    static bool isWorkDay();
    static bool isZhMarketDay();
    static bool isUSMarketDay();
    static bool isHKMarketDay();
    static QString getComCode();
    static void sortByColumn(QList<StockInfo> *mList,const int column,const bool is_asc);
    static void getEastData(QNetworkAccessManager *naManager, QByteArray &allData,float timeOut,const QUrl &url);
    static void postData(QNetworkAccessManager *naManager, QByteArray &postArray, QByteArray &allData,float timeOut, const QUrl &url);
    static QString format_conversion(float data);
    static QString peelStr(const QString s,const QString begin,const QString end);
    static QPair<QString, QString> cutStr(const QString s,const QString begin,const QString end);

    static QString curCode;
    static QString curName;
    static QStringList TableList;
    static float preClose;
    static int WhichInterface;
    static int preInterface;
    static bool isUsZhStock;
    static QString circle_green_SheetStyle;
    static QString circle_red_SheetStyle;
    static QSettings *settings;
    static QList<StockInfo> mTableList;
    static QList<StockInfo> mTableListCopy;
    static QList<StockInfo> mRisingSpeedList;
    static QList<StockInfo> mMyStockList;
    static QList<IndexInfo> mIndexList;
    static QList<timeShareTickInfo> mTimeShareTickList;
    static QList<timeShartChartInfo> mTimeShareChartList;
    static QList<candleChartInfo> mCandleChartList;
    static int KRange;
    static int KBegin;
    static float timeShareHighLowPoint[5];
    static float candleHighLowPoint[5];
    static float buySellPrice[10];
    static float buySellNum[10];
    static float baseInfoData[13];
    static int curSortNum;
    static bool is_asc;
    static QPalette pRed;
    static QPalette pGreen;
    static QPalette pBlack;
    static QPalette pBlue;
    static int upNums;
    static int downNums;
    static bool isNewsReport;
    static bool isFirst;
    static int trendsTotal;
    static bool isSayNews;
    static int curBlock;
    static QString currentPath;
};

#endif // GLOBALVAR_H
