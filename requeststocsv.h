
#ifndef REQUESTSTOCSV_H
#define REQUESTSTOCSV_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QJsonArray>
#include <QTextCodec>

class RequestsToCsv : public QObject
{
    Q_OBJECT
public:
    explicit RequestsToCsv(QObject *parent = nullptr);
    QNetworkAccessManager *naManager;
    QByteArray allData;
    QStringList listJP;
    void getIndexList();
    void getPlateList();
    void dealWithPlateList(QList<QStringList> &list);
    void getStockList();
    void dealWithAllList();
    QString CNToEL(const QString &cnstr);

signals:

};

#endif // REQUESTSTOCSV_H
