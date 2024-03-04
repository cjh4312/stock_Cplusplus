
#ifndef THREADTABLE_H
#define THREADTABLE_H

#include "globalvar.h"
#include <QFile>
#include <QTextCodec>


class ThreadTable : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTable(QObject *parent = nullptr);
    void getTableData();

private:
    void initTableList();
    void readMyStock();
    void reFlaseMyStock();
    void getEastFundFlow();
    bool isFirst=true;
    QByteArray allData;
    int risingSpeedSize=22;
//    QNetworkAccessManager *naManager =new QNetworkAccessManager(this);

signals:
    void getTableDataFinished();
    void getBlockFinished();
};

#endif // THREADTABLE_H
