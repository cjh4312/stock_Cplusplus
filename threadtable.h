
#ifndef THREADTABLE_H
#define THREADTABLE_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QTextCodec>

class ThreadTable : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTable(QObject *parent = nullptr);
    void getTableData();
private:
    QNetworkAccessManager *naManager;
//    bool isFirstReadMyStock=true;
    bool isRunning=false;

    void initTableList(const QByteArray &allData);
    void readMyStock();
    void reFlaseMyStock();

signals:
    void getTableDataFinished();
};

#endif // THREADTABLE_H
