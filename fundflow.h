
#ifndef FUNDFLOW_H
#define FUNDFLOW_H

#include "qlabel.h"
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
#include <complex>
#include <QCheckBox>

class FundFlow : public QTableView
{
public:
    FundFlow();
//    QNetworkAccessManager *naManager=new QNetworkAccessManager(this);
    QList<QStringList> FundFlowList;
    QStandardItemModel *model=new QStandardItemModel(this);
    QWidget *tableChart=new QWidget(this);
    QDialog *fundFlowChart=new QDialog(this);
    QList<QStringList> fundFlowKList;
    QList<QStringList> fundFlowHKList;
    float maxMinKChart[2];
    float maxMinHKChart[2];
    bool isClick=false;
    float pieData[8]={0.0};
    int whichPiece=-1;
    float pointX[8];
    float pointY[8];
    float degree[8]={0.0};
    float fiveTotal[5]={0.0};
    float twentyTotal[5]={0.0};
    bool isShow[6]={true,true,true,true,false,false};
    QCheckBox *checkBox[6];
    QLabel *backGround=new QLabel(fundFlowChart);
    QLabel *vKLine=new QLabel(fundFlowChart);
    QLabel *time=new QLabel(fundFlowChart);
    QLabel *textFund[5];
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
    void initAllNews();
    void getAnnoucement();
    void getNews();
    void getEastNews();
    void getGreatEvent();
    bool isGetNews=false;
};

#endif // FUNDFLOW_H
