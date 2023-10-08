
#ifndef THREADTABLE_H
#define THREADTABLE_H


#include <QObject>
//#include <QNetworkAccessManager>
#include <QFile>
#include <QTextCodec>
#include <QMutex>

class ThreadTable : public QObject
{
    Q_OBJECT
public:
    explicit ThreadTable(QObject *parent = nullptr);
    void getTableData();

private:
    void initTableList(QByteArray allData);
    void readMyStock();
    void reFlaseMyStock();

    QMutex m_mutex;

signals:
    void getTableDataFinished();
};

#endif // THREADTABLE_H
