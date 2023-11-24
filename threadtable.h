
#ifndef THREADTABLE_H
#define THREADTABLE_H

#include <QObject>
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
    QByteArray allData;
//    QString columns[17]={"f12","f14","f2","f3","f8","f6","f22","f9","f20","f21","f25","f24","f5","f15","f16","f17","f18"};

signals:
    void getTableDataFinished();
};

#endif // THREADTABLE_H
