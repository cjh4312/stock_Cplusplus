
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

signals:
    void getTableDataFinished();
};

#endif // THREADTABLE_H
