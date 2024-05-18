
#ifndef REQUESTSTOCSV_H
#define REQUESTSTOCSV_H

#include "qdialog.h"
#include "qpushbutton.h"
//#include "qwidget.h"
#include <QObject>
//#include <QNetworkAccessManager>
#include <QFile>
#include <QJsonArray>
#include <QTextCodec>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

class RequestsToCsv : public QDialog
{
    Q_OBJECT
public:
    explicit RequestsToCsv(QDialog *parent = nullptr);
//    QNetworkAccessManager *naManager=new QNetworkAccessManager(this);
//    QStringList listJP;

    bool getIndexList();
    bool getPlateList();
    void dealWithPlateList(QList<QStringList> &list,const QByteArray &allData);
    QString getStockList();
    void dealWithAllList();
    void downStockIndexPlateInfo();
    void downloadAllStockK();
//    void baoShareStockK();
    QDialog *progressBarWindow = new QDialog();
    QPushButton *stopBtn = new QPushButton("终止下载",progressBarWindow);
    QLabel *numLine = new QLabel(progressBarWindow);
    QProgressBar *progressBar = new QProgressBar(progressBarWindow);
    bool isStop=false;
    QString CNToEL(const QString &cnstr);
    bool isDownload=false;

signals:

};

#endif // REQUESTSTOCSV_H
