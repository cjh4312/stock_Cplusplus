
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
#include <QDialog>
#include <QPainter>

class FundFlow : public QTableView
{
public:
    FundFlow();
//    QNetworkAccessManager *naManager;
    QList<QStringList> FundFlowList;
    QStandardItemModel *model=new QStandardItemModel(this);
    QWidget *tableChart=new QWidget(this);
    QDialog *fundFlowChart=new QDialog(this);
    QList<QStringList> fundFlowKChart;
    QList<QStringList> fundFlowHKChart;
    float maxMinKChart[2];
    float maxMinHKChart[2];
    float pieData[8];
    void getEastPlateFundFlow(int days);
    void getFundFlowChartData(QString code);
    void drawFundFlowChart(QPainter *painter);
    void getData(int days,const QByteArray &allData);
    void getBoardStock(QString name);
    void getIntervalHighLow();
    void drawIntervalHighLowChart(QPainter *painter);
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
    void getGreatEvent();
    bool isGetNews=false;
};

#endif // FUNDFLOW_H
