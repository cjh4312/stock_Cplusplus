#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qgridlayout.h"
#include "tablestock.h"
#include "threadtable.h"
#include "threadindex.h"
#include "threadnewsreport.h"
#include "threadtimesharetick.h"
#include "threadtimesharechart.h"
#include "threadcandlechart.h"
#include "drawchart.h"
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QRadioButton>
#include <QButtonGroup>
#include <QWheelEvent>
#include <QActionGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tradingTimeRunThread();
    void reFlashTable();
    void reFlashIndex();
    void reFlashBuySellBaseInfo();
    void setMarket();
    void addRightMenu(int num);
    void delRightMenu();

signals:
    void startThreadTable();
    void startThreadIndex();
    void startThreadTimeShareTick();
    void startThreadTimeShareChart();
    void startThreadGetNews();
    void startThreadCandleChart();

private:
    Ui::MainWindow *ui;
    TableStock m_tableStock;
    DrawChart drawChart;
    ThreadTable *threadTable;
    ThreadIndex *threadIndex;
    ThreadTimeShareTick *threadTimeShareTick;
    ThreadTimeShareChart *threadTimeShareChart;
    ThreadNewsReport *threadNewsReport;
    ThreadCandleChart *threadCandleChart;

    QLabel *colPrice;
    QLabel *rowTime;
    QLabel *timeSharePrice;

    QThread *thread1;
    QThread *thread2;
    QThread *thread3;
    QThread *thread4;
    QThread *thread5;
    QThread *thread6;
    QTimer *tim;
    QLabel *circle;
    QLabel *baseInfoData[16];
    QLabel *buySellPrice[10];
    QLabel *buySellNum[10];
    QLabel *stockCode;
    QLabel *stockName;
//    QTextEdit *timeShareTick;
    QTextBrowser *newsData;
    int timeCount=0;
    bool changeInTurn=true;

    void initThread();
    void initInterface();
    void initSettings();
    void initBaseInfoLayout(QGridLayout *baseInfoLayout);
    void initBuySellLayout(QGridLayout *BuySellLayout);
    void initSignals();
    void saveMyStock();
//    void drawTimeShareChart();
//    void drawCandleChart();
    void flashOldCandleInfo(QMouseEvent *mouseEvent);

protected:
    bool eventFilter(QObject *obj, QEvent *event);//事件过滤器
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
};
#endif // MAINWINDOW_H
