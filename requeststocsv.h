
#ifndef REQUESTSTOCSV_H
#define REQUESTSTOCSV_H

#include "qdialog.h"
//#include "qwidget.h"
#include <QObject>
//#include <QNetworkAccessManager>
#include <QFile>
#include <QJsonArray>
#include <QTextCodec>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QProgressBar>

class RequestsToCsv : public QDialog
{
    Q_OBJECT
public:
    explicit RequestsToCsv(QDialog *parent = nullptr);
//    QNetworkAccessManager *naManager;
//    QStringList listJP;
    void getIndexList();
    void getPlateList();
    void dealWithPlateList(QList<QStringList> &list,const QByteArray &allData);
    void getStockList();
    void dealWithAllList();
    void downStockIndexPlateInfo();
    void downloadAllStockK();
//    void baoShareStockK();
    bool isStop=false;
    QString CNToEL(const QString &cnstr);
    bool isDownload=false;

signals:

};

#endif // REQUESTSTOCSV_H
