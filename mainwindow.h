#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qheaderview.h"
#include "qspinbox.h"
#include "stockinfo.h"
#include "jspickstock.h"
#include "qgridlayout.h"
#include "tablestock.h"
#include "threadtable.h"
#include "threadindex.h"
#include "threadnewsreport.h"
#include "threadtimesharetick.h"
#include "threadtimesharechart.h"
#include "threadcandlechart.h"
#include "drawchart.h"
#include "searchstock.h"
#include "requeststocsv.h"
#include "f10view.h"
#include "fundflow.h"
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QRadioButton>
#include <QButtonGroup>
#include <QWheelEvent>
#include <QActionGroup>
#include <QTextBlock>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QListWidgetItem>
// #include <complex>
#include <QProcess>
#undef slots
#include <Python.h>
#define slots Q_SLOTS

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
    void reFlashIndex();
    void reFlashBuySellBaseInfo();
    void setMarket();
    void addRightMenu(int num);
    void showSearchResult();
    void resetKParameter();
    void dealWithFundFlow();
    void fastTrade();
    void delMyStock();
    void updateFeeling(QDate date);

signals:
    void startThreadTable();
    void startThreadIndex();
    void startThreadTimeShareTick(bool reset);
    void startThreadTimeShareChart(bool reset);
    void startThreadGetNews();
    void startThreadCandleChart(QString freq,QString adjustFlag,bool isFirst);

private:
    Ui::MainWindow *ui;
    QProcess *QMTProcess=new QProcess(this);
    TableStock mTableStock;
    DrawChart drawChart;
    SearchStock searchStock;
    RequestsToCsv requestsToCsv;
    F10View f10View;
    FundFlow mFundFlow;
    JSPickStock *mPickStock=new JSPickStock;
    ThreadTable *threadTable;
    ThreadIndex *threadIndex;
    ThreadTimeShareTick *threadTimeShareTick;
    ThreadTimeShareChart *threadTimeShareChart;
    ThreadNewsReport *threadNewsReport;
    ThreadCandleChart *threadCandleChart;
    bool isThreadRunning=false;
    bool isFlashBaseInfo=true;
    PyObject* pModule;

    QThread *thread[6];
    QTimer *tim;
    QLabel *circle;
    QLabel *baseInfoData[14];
    QLabel *buySellPrice[10];
    QLabel *buySellNum[10];
    QLabel *feelingData[8];
    QLabel *stockCode;
    QLabel *stockName;
    QTextBrowser *newsData;
    QWidget *leftWindow;
    QScrollBar *leftHorizontalSCrollBar;
    int HorizontalSCrollBarMax=0;
    bool isFirstSCrollBar=true;
    int leftHorizontalSCrollBarPos=0;
    int hideCurCol=2;
    QScrollBar *myStockHorizontalSCrollBar;
    int myHorizontalSCrollBarMax=0;
    bool isFirstMySCrollBar=true;
    int myHorizontalSCrollBarPos=0;
    int myHideCurCol=2;
    QWidget *middleWindow;
    QWidget *myStockWindow;
    QWidget *rightBaseWindow;
    QWidget *rightFundWindow;
    QWidget *searchSmallWindow;
    QWidget *F10SmallWindow;
    QPointF p;
    int hisTimeShareN;
    QString account;

    int timeCount=-3;
    QString downloadDate;
    QString feelingDate;
    bool changeInTurn=true;
    bool isAsia=true;
    bool isTraversalMyStock=false;
    int ifCanClick=1;
    QString freq="101";
    QString adjustFlag="0";
    QRadioButton *periodAdjust[6];
    QPushButton *F10Info[6];
    QPushButton *fundFlow[10];
    QString preCode="";
    QLabel *EPSLabel;
    QLabel *PELabel;
    QLabel *fTitle=new QLabel("标题栏",this);
    QComboBox *periodBox=new QComboBox(this);
    QDateEdit *dateEdit=new QDateEdit(this);
    QDateEdit *dateEdit1=new QDateEdit(this);
    QComboBox *northBox=new QComboBox(this);
    QComboBox *tradedetailBox=new QComboBox(this);
    QComboBox *singleStockBoard=new QComboBox(this);
    QComboBox *openFundBox=new QComboBox(this);
    bool preSort=false;
    float tradePrice;
    int howPosition=0;

    void initGlobalVar();
    void initThread();
    void initInterface();
    void initSettings();
    void initBaseInfoLayout(QGridLayout *baseInfoLayout);
    void initBuySellLayout(QGridLayout *BuySellLayout);
    void initFeelingLayout(QGridLayout *feelingLayout);
    void initSignals();
    void saveCode();
    void flashOldCandleInfo(QMouseEvent *mouseEvent);
    void toInterFace(QString which);
    void toFundFlow();
    void downUpLookStock(QWheelEvent *event);


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};
#endif // MAINWINDOW_H
