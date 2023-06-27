
#ifndef FUNDFLOW_H
#define FUNDFLOW_H

#include "stockinfo.h"
#include <QTableView>
#include <QStandardItemModel>
//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QFile>
#include <QTextCodec>
//#include <QStringRef>
#include <QJSEngine>
#include <QNetworkCookie>

class FundFlow : public QTableView
{
public:
    FundFlow();
//    QNetworkAccessManager *naManager;
    QList<QStringList> FundFlowList;
    QStandardItemModel *model=new QStandardItemModel(this);
    void getEastPlateFundFlow(int days);
    void getData(int days,const QByteArray &allData);
    void getBoardStock(QString name);
    void getIntervalHighLow();
    void getStockPoolStrong(QString date);
    void getNorthFundFlow(QString days);
    void getDragonTigerList(int nums,int pages);
    void countSingleStockBoard(QString nums,int pages);
    void getStockHot();
    void openFundRank(QString ft,QString sc);
    void getRoyalFlushFundFlow();
    void getNotNormalStock();
    void getHisTimeShareTick();
    void getTimeShareMin(QString code,QString date);
    void getVacation();
    void initAllNews();
    void getAnnoucement();
    void getNews();
    void getEastNews();
};

#endif // FUNDFLOW_H
