
#ifndef GETF10INFO_H
#define GETF10INFO_H

//#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QTextCodec>
#include <QFile>
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "globalvar.h"

class GetF10Info : public QObject
{
    Q_OBJECT
public:
    GetF10Info(QObject *parent = nullptr);
//    QNetworkAccessManager *naManager=new QNetworkAccessManager(this);
    QList<QStringList> f10QList;
    QStringList colList;
    QStringList period={};
    QString companyType="4";
    void getStockHotRank();
    void getMainIndex();
    void mainBusinessComposition();
    void eastMainBusinessComposition();
    void assetLiability();
    void getIncome();
    void getCashFlow();
    void getData(const QByteArray &allData,QStringList key,QStringList value,QStringList &col,QList<QStringList> &l);
    void getAllData(QStringList key,QStringList value,QString url);
    void calcPeriod();
};

#endif // GETF10INFO_H
