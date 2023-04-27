#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globalvar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initThread();
    initInterface();
    initSettings();
    initSignals();
    tim = new QTimer();
    tim->setInterval(500);
    connect(tim,SIGNAL(timeout()),this,SLOT(tradingTimeRunThread()));
    tim->start();
}

MainWindow::~MainWindow()
{
    saveCode();
    thread1->quit();
    thread1->wait();
    threadTable->deleteLater();
    thread2->quit();
    thread2->wait();
    threadIndex->deleteLater();
    thread3->quit();
    thread3->wait();
    threadTimeShareTick->deleteLater();
    thread4->quit();
    thread4->wait();
    threadTimeShareChart->deleteLater();
    thread5->quit();
    thread5->wait();
    threadNewsReport->deleteLater();
    thread6->quit();
    thread6->wait();
    threadCandleChart->deleteLater();
    delete ui;
}

void MainWindow::initThread()
{
    thread1 = new QThread(this);
    threadTable=new ThreadTable;
    threadTable->moveToThread(thread1);
    connect(threadTable,SIGNAL(getTableDataFinished()),this,SLOT(reFlashTable()));
    connect(this,&MainWindow::startThreadTable,threadTable,&ThreadTable::getTableData);
    thread1->start();
    emit startThreadTable();

    thread2 = new QThread(this);
    threadIndex=new ThreadIndex;
    threadIndex->moveToThread(thread2);
    connect(threadIndex,SIGNAL(getIndexFinished()),this,SLOT(reFlashIndex()));
    connect(this,&MainWindow::startThreadIndex,threadIndex,&ThreadIndex::getAllIndex);
    thread2->start();
    emit startThreadIndex();

    QString currentPath=QCoreApplication::applicationDirPath()+"/config.ini";
    QSettings *settings=new QSettings(currentPath,QSettings::IniFormat);
    GlobalVar::curCode=settings->value("curCode").toString();
    GlobalVar::curName=settings->value("curName").toString();
    GlobalVar::isSayNews=settings->value("sayNews").toBool();

    settings->deleteLater();
    settings=nullptr;
    thread3 = new QThread(this);
    threadTimeShareTick=new ThreadTimeShareTick;
    threadTimeShareTick->moveToThread(thread3);
    connect(threadTimeShareTick,&ThreadTimeShareTick::getTimeShareTickFinished,this,[=](){
        m_tableStock.setTimeShareTickView();
    });
    connect(threadTimeShareTick,SIGNAL(getBuySellFinished()),this,SLOT(reFlashBuySellBaseInfo()));
    connect(this,&MainWindow::startThreadTimeShareTick,threadTimeShareTick,&ThreadTimeShareTick::getBuySellTimeShareTick);
    thread3->start();
    emit startThreadTimeShareTick();

    thread4 = new QThread(this);
    threadNewsReport=new ThreadNewsReport;
    threadNewsReport->moveToThread(thread4);
    connect(threadNewsReport,&ThreadNewsReport::getNewsFinished,this,[=](QString s){
        newsData->append(s);
});
    connect(this,&MainWindow::startThreadGetNews,threadNewsReport,&ThreadNewsReport::getNewsData);
    thread4->start();
    emit startThreadGetNews();

    thread5 = new QThread(this);
    threadTimeShareChart=new ThreadTimeShareChart;
    threadTimeShareChart->moveToThread(thread5);
    connect(threadTimeShareChart,&ThreadTimeShareChart::getTimeShareChartFinished,this,[=](){
        drawChart.timeShareChart->update();
    });
    connect(this,&MainWindow::startThreadTimeShareChart,threadTimeShareChart,&ThreadTimeShareChart::getAllTimeShareChart);
    thread5->start();
    emit startThreadTimeShareChart();

    thread6 = new QThread(this);
    threadCandleChart=new ThreadCandleChart;
    threadCandleChart->moveToThread(thread6);
    connect(threadCandleChart,&ThreadCandleChart::getCandleChartFinished,this,[=](){
        drawChart.candleChart->update();
//        drawChart.candleChart->show();
//        m_tableStock.stockTableView->hide();
//        m_tableStock.risingSpeedView->hide();
//        m_tableStock.myStockView->hide();
    });
    connect(this,&MainWindow::startThreadCandleChart,threadCandleChart,&ThreadCandleChart::getAllCandleChart);
    thread6->start();
//    emit startThreadCandleChart(freq,adjustFlag);

}
void MainWindow::initInterface()
{
    ui->setupUi(this);
    setWindowTitle("Stock");
    setWindowIcon(QIcon(":/new/pictures/logo.ico"));
    if (not GlobalVar::isSayNews)
        ui->newsReport->setChecked(true);
//    QActionGroup *market = new QActionGroup(this);

//    market->addAction(ui->HKMarket);
//    market->addAction(ui->USMarket);
//    market->addAction(ui->ZHMarket);
//    market->addAction(ui->USzMarket);
    //所有股票界面
    ui->mainLayout->setContentsMargins(2,2,2,2);
    ui->mainLayout->addWidget(m_tableStock.stockTableView);

    ui->mainLayout->addWidget(drawChart.candleChart);
    drawChart.candleChart->hide();

    searchSmallWindow=new QWidget(this);
    QVBoxLayout *search =new QVBoxLayout();
    search->setContentsMargins(2, 2, 2, 2);
    searchSmallWindow->setLayout(search);
    search->addWidget(searchStock.searchCodeLine);
    search->addWidget(searchStock.matchCodeText);

    //中间界面
    QVBoxLayout *mainLayout2 =new QVBoxLayout();
    mainLayout2->setSpacing(0);
    mainLayout2->setContentsMargins(0,0,0,0);
    m_tableStock.risingSpeedView->setMaximumWidth(530);
    m_tableStock.risingSpeedView->setMinimumHeight(494);
    m_tableStock.myStockView->setMaximumWidth(530);
    mainLayout2->addWidget(m_tableStock.risingSpeedView);
    mainLayout2->addWidget(m_tableStock.myStockView);
    //右边界面
    QWidget *right=new QWidget(this);
    right->setMaximumWidth(450);
    QVBoxLayout *rightLayout =new QVBoxLayout();
    right->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0,0,0,0);

    ui->mainLayout->addLayout(mainLayout2);
    ui->mainLayout->addWidget(right);
    //右边界面布局
    QGridLayout *baseInfoLayout=new QGridLayout();
    baseInfoLayout->setSpacing(2);
    baseInfoLayout->setContentsMargins(10, 0, 0, 0);
    QGridLayout *buySellLayout=new QGridLayout();
    buySellLayout->setSpacing(2);
    buySellLayout->setContentsMargins(10, 5, 0, 0);
    newsData=new QTextBrowser(this);
//    newsData->setMinimumHeight(140);

    QHBoxLayout *freqAdjustLayout =new QHBoxLayout();
    freqAdjustLayout->setSpacing(0);

    rightLayout->addLayout(baseInfoLayout);
    rightLayout->addLayout(buySellLayout);
    rightLayout->addWidget(newsData);
    rightLayout->addLayout(freqAdjustLayout);
    QButtonGroup *freq=new QButtonGroup(this);
    daily =new QRadioButton("日线");
    daily->setChecked(true);
    weekly =new QRadioButton("周线");
    monthly =new QRadioButton("月线");
    freq->addButton(daily);
    freq->addButton(weekly);
    freq->addButton(monthly);
    no_adjust =new QRadioButton("不复权");
    no_adjust->setChecked(true);
    split_adjusted =new QRadioButton("前复权");
    back_adjusted =new QRadioButton("后复权");
    freqAdjustLayout->addWidget(daily);
    freqAdjustLayout->addWidget(weekly);
    freqAdjustLayout->addWidget(monthly);
    freqAdjustLayout->addWidget(no_adjust);
    freqAdjustLayout->addWidget(split_adjusted);
    freqAdjustLayout->addWidget(back_adjusted);

    drawChart.timeShareChart->setMinimumHeight(300);
    rightLayout->addWidget(drawChart.timeShareChart);

    initBaseInfoLayout(baseInfoLayout);
    initBuySellLayout(buySellLayout);
    //状态栏
    circle=new QLabel(this);

    ui->statusBar->addWidget(circle);
    for(int i=1;i<=7;++i)
    {
//        QHBoxLayout *statusBarLayout=new QHBoxLayout(ui->statusBar);
//        ui->statusBar->setLayout(statusBarLayout);
        QPushButton *indexButton = new QPushButton(QString::asprintf("Button%d", i), this);
        indexButton->setStyleSheet("QPushButton{font:bold;font-size:16px;font-family:微软雅黑;}");
        QLabel *indexLabel=new QLabel(QString::asprintf("Label%d", i), this);
        indexLabel->setStyleSheet("QLabel{font:bold;font-size:14px;}");
        ui->statusBar->addWidget(indexButton);
        ui->statusBar->addWidget(indexLabel);
    }
}

void MainWindow::initSettings()
{
    m_tableStock.stockTableView->verticalScrollBar()->installEventFilter(this);
    newsData->verticalScrollBar()->installEventFilter(this);
    m_tableStock.timeShareTickView->verticalScrollBar()->installEventFilter(this);
    drawChart.timeShareChart->installEventFilter(this);
    drawChart.candleChart->installEventFilter(this);
    drawChart.candleChart->setMouseTracking(true);

    m_tableStock.stockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableStock.risingSpeedView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableStock.myStockView->setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    m_tableStock.stockTableView->setFocusPolicy(Qt::NoFocus);
    m_tableStock.risingSpeedView->setFocusPolicy(Qt::NoFocus);
    m_tableStock.myStockView->setFocusPolicy(Qt::NoFocus);

    colPrice=new QLabel(drawChart.candleChart);
    rowTime=new QLabel(drawChart.candleChart);
    colPrice->resize(72,15);
    colPrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
    colPrice->setAlignment(Qt::AlignCenter);
    rowTime->resize(95,15);
    rowTime->setStyleSheet("color:yellow;font:bold;font-size:18px");
    rowTime->setAlignment(Qt::AlignCenter);
    timeSharePrice=new QLabel(drawChart.timeShareChart);
    drawChart.timeShareChart->setStyleSheet("color:white;font:bold;font-size:14px");
    timeSharePrice->setAlignment(Qt::AlignCenter);
    timeSharePrice->resize(110,15);

    searchSmallWindow->setWindowFlag(Qt::Popup);
    searchSmallWindow->setGeometry(1466, 645, 280, 350);
    searchStock.searchCodeLine->installEventFilter(this);

}
void MainWindow::initBaseInfoLayout(QGridLayout *baseInfoLayout)
{
    stockCode = new QLabel();
    stockName = new QLabel();
    stockCode->setStyleSheet("QLabel{font:bold;color:blue}");
    stockName->setStyleSheet("QLabel{font:bold 26px;font-family:微软雅黑;color:red}");
    baseInfoLayout->addWidget(stockCode, 0, 0, 2, 1);
    baseInfoLayout->addWidget(stockName, 0, 1, 2, 3);

    QString lName[]={"现价", "涨幅","换手", "成交额", "总股本", "总市值", "PE(静)", "上涨总数",
                       "今开", "最高", "最低", "成交量(手)", "流通股", "", "PE(动)", "下跌总数"};

    for (int i=0;i<16;++i)
    {
        QLabel *name=new QLabel(lName[i]);
        baseInfoData[i]=new QLabel();
        name->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑}");
        if ((i >= 2 and i <= 6) or (i >= 11 and i <= 14))
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:blue}");
        else if (i == 7)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:red}");
//            name->setMaximumWidth(90);
        }
        else if (i == 15)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:green}");
//            name->setMaximumWidth(90);
        }
        else
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px}");
        if (i < 8)
        {
            baseInfoLayout->addWidget(name,i+2,0);
            baseInfoLayout->addWidget(baseInfoData[i],i+2,1);
        }
        else
        {
            baseInfoLayout->addWidget(name,i-6,2);
            baseInfoLayout->addWidget(baseInfoData[i],i-6,3);
        }
    }
}
void MainWindow::initBuySellLayout(QGridLayout *BuySellLayout)
{
    QString n[5]={"一","二", "三", "四", "五"};
    QLabel *buySellName[10];
    for (int i=0;i<5;++i)
    {
        buySellName[i]=new QLabel("卖"+n[4-i]);
        buySellPrice[i]=new QLabel();
        buySellNum[i]=new QLabel();
        buySellNum[i]->setAlignment(Qt::AlignRight);
        buySellName[i]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑}");
        buySellPrice[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i]->setStyleSheet("QLabel{font:16px;color:blue;font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i],i,0);
        BuySellLayout->addWidget(buySellPrice[i],i,1);
        BuySellLayout->addWidget(buySellNum[i],i,2);

        buySellName[i+5]=new QLabel("买"+n[i]);
        buySellPrice[5+i]=new QLabel();
        buySellNum[5+i]=new QLabel();
        buySellNum[5+i]->setAlignment(Qt::AlignRight);
        buySellName[i+5]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:blue}");
        buySellPrice[i+5]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i+5]->setStyleSheet("QLabel{font:16px;color:blue;font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i+5],i+6,0);
        BuySellLayout->addWidget(buySellPrice[5+i],i+6,1);
        BuySellLayout->addWidget(buySellNum[5+i],i+6,2);
    }
    buySellName[0]->setMinimumWidth(40);
    buySellPrice[0]->setMinimumWidth(45);
    buySellNum[0]->setMinimumWidth(40);
    QFrame *line=new QFrame(this);
    line->setStyleSheet("QFrame{background:yellow;min-height:0px}");
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    BuySellLayout->addWidget(line,5,0,1,3);

//    timeShareTick =new QTextEdit();
//    timeShareTick->setMinimumHeight(240);
//    timeShareTick->setMinimumWidth(320);
//    timeShareTick->setReadOnly(true);
    m_tableStock.timeShareTickView->setMinimumWidth(300);
    BuySellLayout->addWidget(m_tableStock.timeShareTickView,0,3,11,3);
}
void MainWindow::initSignals()
{
    connect(m_tableStock.stockTableView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
        GlobalVar::curName=GlobalVar::mTableList.at(curRow).name;
        emit startThreadTimeShareTick();
        emit startThreadTimeShareChart();
    });
    connect(m_tableStock.risingSpeedView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mRisingSpeedList.at(curRow).code;
        GlobalVar::curName=GlobalVar::mRisingSpeedList.at(curRow).name;
        emit startThreadTimeShareTick();
        emit startThreadTimeShareChart();
    });
    connect(m_tableStock.myStockView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
        GlobalVar::curName=GlobalVar::mMyStockList.at(curRow).name;
        emit startThreadTimeShareTick();
        emit startThreadTimeShareChart();
    });
    connect(ui->ZHMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->HKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USzMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(m_tableStock.stockTableView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::WhichInterface=3;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mTableList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag);
        drawChart.candleChart->show();
        m_tableStock.stockTableView->hide();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
    });
    connect(m_tableStock.risingSpeedView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::WhichInterface=3;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mRisingSpeedList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mRisingSpeedList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag);
        drawChart.candleChart->show();
        m_tableStock.stockTableView->hide();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
    });
    connect(m_tableStock.myStockView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::WhichInterface=3;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mMyStockList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag);
        drawChart.candleChart->show();
        m_tableStock.stockTableView->hide();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
    });
    connect(m_tableStock.stockTableView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        addRightMenu(1);
    });
    connect(m_tableStock.risingSpeedView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        addRightMenu(2);
    });
    connect(m_tableStock.myStockView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        delRightMenu();
    });
    for (int i = 2; i<=14; i=i+2)
    {
        QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(i));
        connect(bb,&QPushButton::clicked,this,[=](){
            GlobalVar::WhichInterface=3;
            QString IndexCode[]={"1.000001","399001","399006","100.HSI","100.N225","100.KS11","1.000688","100.TWII","100.SENSEX","100.DJIA",
                                "100.SPX","100.NDX","100.SX5E","100.GDAXI","100.RTS","100.FTSE","100.FCHI","100.AS51","104.CN00Y","103.YM00Y"};
            int n;
            if (GlobalVar::preInterface==1 or GlobalVar::preInterface==2)
            {
                if ((i==8 or i==10 or i==12) and not changeInTurn)
                    n=i/2+2;
                else
                    n=i/2-1;
            }
            else
            {
                if ((i==8 or i==10 or i==12) and not changeInTurn)
                    n=i/2+11;
                else
                    n=i/2+8;
            }
            if (i==14)
            {
                if (changeInTurn)
                    n=18;
                else
                    n=19;
            }
//            qDebug()<<i<<n;
            GlobalVar::curCode=IndexCode[n];
            GlobalVar::curName=bb->text();
            emit startThreadCandleChart(freq,adjustFlag);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
            drawChart.candleChart->show();
            m_tableStock.stockTableView->hide();
            m_tableStock.risingSpeedView->hide();
            m_tableStock.myStockView->hide();
        });
    }
    connect(ui->newsReport,&QAction::triggered,this,[=](){
        if (threadNewsReport->tts->state() == QTextToSpeech::Speaking)
        {
            threadNewsReport->tts->stop();
            threadNewsReport->tts->resume();
        }
        GlobalVar::isSayNews=not GlobalVar::isSayNews;
    });
    connect(&searchStock,SIGNAL(showSearch()),this,SLOT(showSearchResult()));
    connect(ui->DLStockInfo,&QAction::triggered,this,&MainWindow::downStockIndexPlateInfo);
    connect(daily,&QRadioButton::clicked,this,&MainWindow::initFlag);
    connect(weekly,&QRadioButton::clicked,this,&MainWindow::initFlag);
    connect(monthly,&QRadioButton::clicked,this,&MainWindow::initFlag);
    connect(no_adjust,&QRadioButton::clicked,this,&MainWindow::initFlag);
    connect(split_adjusted,&QRadioButton::clicked,this,&MainWindow::initFlag);
    connect(back_adjusted,&QRadioButton::clicked,this,&MainWindow::initFlag);
}

void MainWindow::saveMyStock()
{
    QStringList s;
    for(int i=0;i<GlobalVar::mMyStockList.count();++i)
    {
        s<<GlobalVar::mMyStockList.at(i).code;
    }
    GlobalVar::settings->setValue("myStock",s);
}

void MainWindow::saveCode()
{
    if (GlobalVar::curCode.length()!=5 and GlobalVar::curCode.left(1)!="1")
    {
        GlobalVar::settings->setValue("curCode",GlobalVar::curCode);
        GlobalVar::settings->setValue("curName",GlobalVar::curName);
        GlobalVar::settings->sync();
    }
}

void MainWindow::initFlag()
{
    if (sender()==daily)
        freq="101";
    else if (sender()==weekly)
        freq="102";
    else if (sender()==monthly)
        freq="103";
    else if (sender()==no_adjust)
        adjustFlag="0";
    else if (sender()==split_adjusted)
        adjustFlag="1";
    else if (sender()==back_adjusted)
        adjustFlag="2";
    if (GlobalVar::WhichInterface==3)
        emit startThreadCandleChart(freq,adjustFlag);
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_tableStock.stockTableView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();//获得鼠标滚轮的滚动距离para，para<0向下滚动，>0向上滚动
        int tempStep=m_tableStock.stockTableView->verticalScrollBar()->value();
        int curIndex=m_tableStock.stockTableView->currentIndex().row();
        int row=int(m_tableStock.stockTableView->height()/22.0+0.5);
        if (para<0)
        {
            //向下滚动，设定鼠标滚轮每滚动一个单位，滑块就移动20个单位
            m_tableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>GlobalVar::mTableList.count()-row)
                m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(0,0));
            else
                m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(curIndex+row,0));
        }
        else
        {
            //向上滚动
            m_tableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(curIndex-row,0));
            else
                m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(GlobalVar::mTableList.count()-1,0));
        }
//        qDebug()<<m_tableStock.stockTableView->height();
        return true;
    }
    else if (obj==drawChart.timeShareChart and event->type() == QEvent::Paint)
    {
        drawChart.drawTimeShareChart();
        return true;
    }
    else if(obj==drawChart.candleChart and event->type()==QEvent::Paint)
    {
        drawChart.drawCandleChart();
        return true;
    }
    else if(obj==drawChart.candleChart and event->type()==QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        flashOldCandleInfo(mouseEvent);
        return true;
    }
    else if(obj==drawChart.candleChart and event->type()==QEvent::Leave)
    {
        colPrice->hide();
        rowTime->hide();
        reFlashBuySellBaseInfo();
        return true;
    }
    else if(obj==drawChart.timeShareChart and event->type()==QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        float price=GlobalVar::timeShareHighLowPoint[0]-(GlobalVar::timeShareHighLowPoint[0]-GlobalVar::timeShareHighLowPoint[1])*(mouseEvent->pos().ry()-TOPHEIGHTEDGE)/(drawChart.timeShareChart->height()*12/15-2*TOPHEIGHTEDGE);
        timeSharePrice->setText(QString::number((1+price/100)*GlobalVar::preClose,'f',2)+"("+QString::number(price,'f',2)+")%");
        timeSharePrice->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*timeSharePrice->width()/(drawChart.timeShareChart->width()-2*WIDTHEDGE),mouseEvent->pos().ry()-15);
        timeSharePrice->show();
        if (mouseEvent->pos().ry()>drawChart.timeShareChart->height()*12/15)
            timeSharePrice->hide();
        return true;
    }
    else if(obj==drawChart.timeShareChart and event->type()==QEvent::Leave)
    {
        timeSharePrice->hide();
        return true;
    }
    else if(obj==newsData->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
//        int maxStep=newsData->verticalScrollBar()->maximum();
        int tempStep=newsData->verticalScrollBar()->value();
//        qDebug()<<para<<tempStep;
        if (para<0/* and tempStep+6<maxStep*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep+163);
        else if (para>0/* and tempStep>6*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep-163);
        return true;
    }
    else if(obj==m_tableStock.timeShareTickView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=m_tableStock.timeShareTickView->verticalScrollBar()->value();
        if (para<0)
            m_tableStock.timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep+13);
        else
            m_tableStock.timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep-13);
        return true;
    }
    else if (obj==searchStock.searchCodeLine and event->type() ==QKeyEvent::KeyPress)
    {
        QKeyEvent *event1 = static_cast<QKeyEvent *>(event);
        if (event1->key()==Qt::Key_Down)
        {
            GlobalVar::curBlock+=1;
            if (GlobalVar::curBlock>searchStock.matchCodeText->document()->blockCount()-1)
                GlobalVar::curBlock=0;
        }
        else if (event1->key()==Qt::Key_Up)
        {
            GlobalVar::curBlock-=1;
            if (GlobalVar::curBlock<0)
                GlobalVar::curBlock=searchStock.matchCodeText->document()->blockCount()-1;
        }
        QTextBlockFormat fmt=QTextBlockFormat();
        QTextCharFormat charFmt=QTextCharFormat();
        fmt.setBackground(QColor("white"));
        searchStock.matchCodeText->textCursor().setBlockFormat(fmt);
        searchStock.matchCodeText->textCursor().deletePreviousChar();
        searchStock.matchCodeText->textCursor().deletePreviousChar();
        searchStock.matchCodeText->textCursor().deletePreviousChar();
        searchStock.matchCodeText->textCursor().deletePreviousChar();
//        qDebug()<<GlobalVar::curBlock;
        QTextBlock block=searchStock.matchCodeText->document()->findBlockByNumber(GlobalVar::curBlock);
        searchStock.matchCodeText->setTextCursor(QTextCursor(block));
        fmt=searchStock.matchCodeText->textCursor().blockFormat();

        fmt.setBackground(QColor(0, 199, 255));
        searchStock.matchCodeText->textCursor().setBlockFormat(fmt);
        charFmt.setForeground(QColor("red"));
        searchStock.matchCodeText->mergeCurrentCharFormat(charFmt);
        searchStock.matchCodeText->textCursor().insertText(">>> ");
    }
    //其它事件交基类处理
    return QMainWindow::eventFilter(obj, event);
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton)
        if (GlobalVar::WhichInterface==1 or GlobalVar::preInterface==1)
            addRightMenu(3);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QKeySequence key=event->key();
    if (key==Qt::Key_Escape)
    {
        initKParameter();
        if (GlobalVar::WhichInterface==3)
        {
            GlobalVar::WhichInterface=GlobalVar::preInterface;
            drawChart.candleChart->hide();
            m_tableStock.stockTableView->show();
            if (GlobalVar::preInterface==1)
            {
                m_tableStock.risingSpeedView->show();
                m_tableStock.myStockView->show();
            }
        }
    }
    else if (key==Qt::Key_PageDown)
    {
        if (GlobalVar::WhichInterface==3)
        {
            initKParameter();
            int curRow=m_tableStock.stockTableView->currentIndex().row()+1;
            if (curRow>GlobalVar::mTableList.count()-1)
                curRow=0;
            GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
            GlobalVar::curName=GlobalVar::mTableList.at(curRow).name;
            m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(curRow,0));
            //            qDebug()<<GlobalVar::curCode;
            emit startThreadCandleChart(freq,adjustFlag);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
    }
    else if (key==Qt::Key_PageUp)
    {
        if (GlobalVar::WhichInterface==3)
        {
            initKParameter();
            int curRow=m_tableStock.stockTableView->currentIndex().row()-1;
            if (curRow<0)
                curRow=GlobalVar::mTableList.count()-1;
            GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
            GlobalVar::curName=GlobalVar::mTableList.at(curRow).name;
            m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(curRow,0));
            //            qDebug()<<GlobalVar::curCode;
            emit startThreadCandleChart(freq,adjustFlag);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
    }
    else if (key==Qt::Key_Enter or key==Qt::Key_Return)
    {

        GlobalVar::WhichInterface=3;
        emit startThreadCandleChart(freq,adjustFlag);
        drawChart.candleChart->show();
        m_tableStock.stockTableView->hide();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
    }
    else if ((key >= 48 and key <= 57) or (key >= 65 and key <= 90))
    {
//        qDebug()<<event->text();
//        searchCode+=event->text();
        searchSmallWindow->show();
        searchStock.searchCodeLine->setText(event->text());
        searchStock.searchCodeLine->setFocus();
        searchStock.matchCodeText->moveCursor(QTextCursor::Start);
    }
    else if (key==Qt::Key_Up)
    {
        if (GlobalVar::WhichInterface==3)
        {
            GlobalVar::KRange=GlobalVar::KRange*0.8;
            if (GlobalVar::KRange<50)
            {
                GlobalVar::KRange=50;
                return;
            }
            drawChart.candleChart->update();
        }
    }
    else if (key==Qt::Key_Down)
    {
        if (GlobalVar::WhichInterface==3)
        {
            GlobalVar::KRange=GlobalVar::KRange*1.2;
            if (GlobalVar::KRange>GlobalVar::offsetLocal)
                GlobalVar::KRange=GlobalVar::offsetLocal;
            drawChart.candleChart->update();
        }
    }
    else if (key==Qt::Key_Left)
    {
        if (GlobalVar::WhichInterface==3)
        {

            GlobalVar::offsetLocal=GlobalVar::offsetLocal+GlobalVar::KRange*0.2;
            if (GlobalVar::mCandleChartList.count()<GlobalVar::offsetLocal)
                GlobalVar::offsetLocal=GlobalVar::mCandleChartList.count();
//            qDebug()<<GlobalVar::offsetLocal;
            GlobalVar::offsetEnd=0;
            drawChart.candleChart->update();
//            GlobalVar::offsetEnd=OFFSET;
        }
    }
    else if (key==Qt::Key_Right)
    {
        if (GlobalVar::WhichInterface==3)
        {
            GlobalVar::offsetLocal=GlobalVar::offsetLocal-GlobalVar::KRange*0.2;
            if (GlobalVar::offsetLocal<GlobalVar::KRange)
                GlobalVar::offsetLocal=GlobalVar::KRange;
            GlobalVar::offsetEnd=0;
            drawChart.candleChart->update();
//            GlobalVar::offsetEnd=OFFSET;
        }
    }
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (GlobalVar::WhichInterface==3)
    {
        initKParameter();
        int curRow;
        if (event->angleDelta().y()<0)
        {
            curRow=m_tableStock.stockTableView->currentIndex().row()+1;
            if (curRow>GlobalVar::mTableList.count()-1)
                curRow=0;
        }
        else
        {
            curRow=m_tableStock.stockTableView->currentIndex().row()-1;
            if (curRow<0)
                curRow=GlobalVar::mTableList.count()-1;
        }
        GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
        GlobalVar::curName=GlobalVar::mTableList.at(curRow).name;
        m_tableStock.stockTableView->setCurrentIndex(m_tableStock.m_tableModel->index(curRow,0));
        //            qDebug()<<GlobalVar::curCode;
        emit startThreadCandleChart(freq,adjustFlag);
        emit startThreadTimeShareChart();
        emit startThreadTimeShareTick();
    }
}
void MainWindow::setMarket()
{
    QObject* obj = sender();
    if (obj->objectName()=="ZHMarket")
    {
        GlobalVar::WhichInterface=1;
        GlobalVar::preInterface=1;
        m_tableStock.stockTableView->show();
        m_tableStock.risingSpeedView->show();
        m_tableStock.myStockView->show();
        drawChart.candleChart->hide();
        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(false);
        ui->ZHMarket->setChecked(true);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode=GlobalVar::settings->value("curCode").toString();
        GlobalVar::curName=GlobalVar::settings->value("curName").toString();
    }
    else if (obj->objectName()=="HKMarket")
    {
        saveCode();
        GlobalVar::WhichInterface=2;
        GlobalVar::preInterface=2;
        m_tableStock.stockTableView->show();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
        drawChart.candleChart->hide();
        ui->HKMarket->setChecked(true);
        ui->USMarket->setChecked(false);
        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="00700";
        GlobalVar::curName="腾讯控股";
    }
    else if (obj->objectName()=="USMarket")
    {
        saveCode();
        GlobalVar::WhichInterface=5;
        GlobalVar::preInterface=5;
        GlobalVar::isUsZhStock=false;
        m_tableStock.stockTableView->show();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
        drawChart.candleChart->hide();
        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(true);
        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="105.MSFT";
        GlobalVar::curName="微软";
    }
    else if (obj->objectName()=="USzMarket")
    {
        saveCode();
        GlobalVar::WhichInterface=5;
        GlobalVar::preInterface=5;
        GlobalVar::isUsZhStock=true;
        m_tableStock.stockTableView->show();
        m_tableStock.risingSpeedView->hide();
        m_tableStock.myStockView->hide();
        drawChart.candleChart->hide();
        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(true);
        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(true);
        GlobalVar::curCode="106.BABA";
        GlobalVar::curName="阿里巴巴";
    }
    emit startThreadTable();
    emit startThreadTimeShareChart();
    emit startThreadTimeShareTick();
    emit startThreadIndex();
}
void MainWindow::addRightMenu(int num)
{
    StockInfo info;
    if (GlobalVar::WhichInterface!=2 && GlobalVar::WhichInterface!=5)
    {
        QMenu *menu=new QMenu();
        QAction *act=new QAction("加入自选");
        menu->addAction(act);
        menu->popup(QCursor::pos());
        if (num==1)
            info=GlobalVar::mTableList.at(m_tableStock.stockTableView->currentIndex().row());
        else if(num==2)
            info=GlobalVar::mRisingSpeedList.at(m_tableStock.risingSpeedView->currentIndex().row());
        else if(num==3)
        {
            int l = 0;
            int r = GlobalVar::mTableListCopy.count() - 1;
            while (l <= r)
            {
                int mid = (l + r) / 2;
                if (GlobalVar::mTableListCopy.at(mid).code == GlobalVar::curCode)
                {
                    info=GlobalVar::mTableListCopy.at(mid);
                    break;
                }
                else if (GlobalVar::mTableListCopy.at(mid).code > GlobalVar::curCode)
                    r = mid - 1;
                else
                    l = mid + 1;
            }
        }
        connect(act,&QAction::triggered,this,[info, this](){
            for (int i=0;i<GlobalVar::mMyStockList.count();++i)
            {
                if (info.code==GlobalVar::mMyStockList.at(i).code)
                    return;
            }
            GlobalVar::mMyStockList.append(info);
            int curIndex=m_tableStock.myStockView->currentIndex().row();
            m_tableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList);
            m_tableStock.myStockView->setModel(m_tableStock.m_myStockModel);
            m_tableStock.myStockView->setCurrentIndex(m_tableStock.m_myStockModel->index(curIndex,0));
            saveMyStock();
        });
    }
}
void MainWindow::delRightMenu()
{
    QMenu *menu=new QMenu();
    QAction *act=new QAction("删除自选");
    menu->addAction(act);
    menu->popup(QCursor::pos());
    connect(act,&QAction::triggered,this,[=](){
        GlobalVar::mMyStockList.removeAt(m_tableStock.myStockView->currentIndex().row());
        m_tableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList);
        m_tableStock.myStockView->setModel(m_tableStock.m_myStockModel);
        saveMyStock();
    });
}

void MainWindow::showSearchResult()
{
    searchSmallWindow->hide();
    GlobalVar::WhichInterface=3;
    emit startThreadCandleChart(freq,adjustFlag);
    emit startThreadTimeShareChart();
    emit startThreadTimeShareTick();
    drawChart.candleChart->show();
    m_tableStock.stockTableView->hide();
    m_tableStock.risingSpeedView->hide();
    m_tableStock.myStockView->hide();
}

void MainWindow::downStockIndexPlateInfo()
{
    QWidget *promptWindow=new QWidget(this);
    QTextEdit *promptText=new QTextEdit(promptWindow);
    promptText->setStyleSheet("background-color: rgb(211, 211, 211);");
    promptWindow->setWindowFlag(Qt::Popup);
    promptWindow->setGeometry(850, 400, 300, 100);
    QVBoxLayout *Vlay=new QVBoxLayout(promptWindow);
    promptWindow->setLayout(Vlay);
    Vlay->addWidget(promptText);
    promptWindow->show();
    promptText->append("开始下载指数、板块、个股信息...请稍等");
    requestsToCsv.dealWithAllList();
    promptText->append("指数、板块、个股信息处理完毕");
    ui->DLStockInfo->setDisabled(true);
}

void MainWindow::initKParameter()
{
    GlobalVar::offsetEnd=OFFSETEND;
    GlobalVar::KRange=KRANGE;
    GlobalVar::offsetLocal=GlobalVar::KRange;
}
//交易日每5秒刷新一次数据
void MainWindow::tradingTimeRunThread()
{
//    int a = timeCount % 10;
    //每1秒，A股交易时段刷新个股买卖及每笔成交
    if (timeCount%2==1)
    {
        if ((GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==3) and GlobalVar::isZhMarketDay())
            if (GlobalVar::curCode.left(1)!="1")
                emit startThreadTimeShareTick();
    }
    //每5秒
    else if (timeCount==8 or timeCount==18)
    {
        //A股交易时段实时刷新所有股票，及分时图
        if (GlobalVar::WhichInterface==1 and GlobalVar::isZhMarketDay())
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            emit startThreadTable();
            emit startThreadTimeShareChart();

        }
        //美股港股交易时段实时刷新所有股票，及分时图、分时成交
        else if ((GlobalVar::WhichInterface==5 and GlobalVar::isUSMarketDay()) or
                 (GlobalVar::WhichInterface==2 and GlobalVar::isHKMarketDay()))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            emit startThreadTable();
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
        //A股交易时段实时刷新k线图
        else if (GlobalVar::WhichInterface==3 and GlobalVar::isZhMarketDay())
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            emit startThreadCandleChart(freq,adjustFlag);
        }
        //标注非交易时段
        else
            circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);

        //工作日
        if (GlobalVar::isWorkDay())
        {
            //更新全球指数
            emit startThreadIndex();
            //每天9点,更新A股指数板块股票信息
            QDateTime curTime=QDateTime::currentDateTime();
            QString d=curTime.date().toString("yyyy-MM-dd");
            if (curTime.time().toString("hh:mm")>="09:00" and d>GlobalVar::settings->value("curTime").toString())
            {
                downStockIndexPlateInfo();
                GlobalVar::settings->setValue("curTime",d);
            }
        }
        else
            reFlashIndex();
        //交换显示指数
        changeInTurn=not changeInTurn;
    }
    //每10秒获取最新新闻信息
    else if (timeCount==20)
    {
        if (not GlobalVar::isNewsReport)
        {
            GlobalVar::isNewsReport=true;
            emit startThreadGetNews();
        }
        timeCount=0;
    }
    timeCount+=1;
//    qDebug()<<timeCount;
}
void MainWindow::reFlashTable()
{
    m_tableStock.setTableView();
    baseInfoData[7]->setText(QString::number(GlobalVar::upNums));
    baseInfoData[15]->setText(QString::number(GlobalVar::downNums));
}
void MainWindow::reFlashIndex()
{
//    qDebug()<<ui->statusBar->children();
    int n;
    for (int i = 2; i<=12; i=i+2)
    {
        QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(i));
        QLabel *bl = (QLabel *)(ui->statusBar->children().at(i+1));
        if (GlobalVar::preInterface==1 or GlobalVar::preInterface==2)
        {
            if ((i==8 or i==10 or i==12) and not changeInTurn)
                n=i/2+2;
            else
                n=i/2-1;
        }
        else
        {
            if ((i==8 or i==10 or i==12) and not changeInTurn)
                n=i/2+11;
            else
                n=i/2+8;
        }
        bb->setText(GlobalVar::mIndexList.at(n).name);
        if (GlobalVar::mIndexList.at(n).pctChg.toFloat()>0)
            bl->setPalette(GlobalVar().pRed);
        else if (GlobalVar::mIndexList.at(n).pctChg.toFloat()<0)
            bl->setPalette(GlobalVar().pGreen);
        else
            bl->setPalette(GlobalVar().pBlack);
        bl->setText(GlobalVar::mIndexList.at(n).close+" "+GlobalVar::mIndexList.at(n).pctChg+"%");
    }
    if (changeInTurn)
        n=19;
    else
        n=18;
    QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(14));
    bb->setText(GlobalVar::mIndexList.at(n).name);
    QLabel *bl = (QLabel *)(ui->statusBar->children().at(16));
    if (GlobalVar::mIndexList.at(n).pctChg.toFloat()>0)
        bl->setPalette(GlobalVar().pRed);
    else if (GlobalVar::mIndexList.at(n).pctChg.toFloat()<0)
        bl->setPalette(GlobalVar().pGreen);
    else
        bl->setPalette(GlobalVar().pBlack);
    bl->setText(GlobalVar::mIndexList.at(n).close+" "+GlobalVar::mIndexList.at(n).pctChg+"%");
}
void MainWindow::reFlashBuySellBaseInfo()
{
    QString str;
    for (int i=0;i<10;++i)
    {
        float price=GlobalVar::buySellPrice[i];
        str=QString::number(price,'f',2);
        if (GlobalVar::buySellPrice[i]==0)
            str="";
        if (price>GlobalVar::preClose)
            buySellPrice[i]->setPalette(GlobalVar::pRed);
        else if (price<GlobalVar::preClose)
            buySellPrice[i]->setPalette(GlobalVar::pGreen);
        else
            buySellPrice[i]->setPalette(GlobalVar::pBlack);

        buySellPrice[i]->setText(str);
        str=QString::number(GlobalVar::buySellNum[i]);
        if (GlobalVar::buySellNum[i]==0)
            str="";
        buySellNum[i]->setText(str+" ");
    }
    if (GlobalVar::baseInfoData[1]>0)
    {
        baseInfoData[0]->setPalette(GlobalVar::pRed);
        baseInfoData[1]->setPalette(GlobalVar::pRed);
    }
    else if(GlobalVar::baseInfoData[1]<0)
    {
        baseInfoData[0]->setPalette(GlobalVar::pGreen);
        baseInfoData[1]->setPalette(GlobalVar::pGreen);
    }
    else
    {
        baseInfoData[0]->setPalette(GlobalVar::pBlack);
        baseInfoData[1]->setPalette(GlobalVar::pBlack);
    }
    for (int i=0;i<6;++i)
        if (i==0)
            baseInfoData[i]->setText(QString::number(GlobalVar::baseInfoData[i],'f',2));
        else if (i==1 or i==2)
            baseInfoData[i]->setText(QString::number(GlobalVar::baseInfoData[i],'f',2)+"%");
        else
            baseInfoData[i]->setText(GlobalVar::format_conversion(GlobalVar::baseInfoData[i]));
    float pct=0;
    for (int i=8;i<15;++i)
        if (i==8 or i==9 or i==10)
        {
            float p=GlobalVar::baseInfoData[i-2];
            if (GlobalVar::preClose==0 or p==0)
                pct=0;
            else
                pct=(p-GlobalVar::preClose)*100/GlobalVar::preClose;
            baseInfoData[i]->setText(QString::number(p)+"("+GlobalVar::format_conversion(pct)+"%)");

            if (p>GlobalVar::preClose)
                baseInfoData[i]->setPalette(GlobalVar::pRed);
            else if(p<GlobalVar::preClose)
                baseInfoData[i]->setPalette(GlobalVar::pGreen);
            else
                baseInfoData[i]->setPalette(GlobalVar::pBlack);
        }
        else
            baseInfoData[i]->setText(GlobalVar::format_conversion(GlobalVar::baseInfoData[i-2]));
    stockCode->setText(GlobalVar::curCode);
    if (GlobalVar::curCode.left(1)=="3" or GlobalVar::curCode.left(1)=="6" or GlobalVar::curCode.left(1)=="0")
        searchStock.findStockArea();
    stockName->setText(GlobalVar::curName);
}

void MainWindow::flashOldCandleInfo(QMouseEvent *mouseEvent)
{
    int m=(mouseEvent->pos().x()-KWIDTHEDGE)*GlobalVar::KRange/(drawChart.candleChart->width()-2*KWIDTHEDGE);
//    qDebug()<<m<<GlobalVar::KRange;
    int n;
    if (GlobalVar::mCandleChartList.count()<GlobalVar::KRange)
        n=m;
    else
        n=GlobalVar::mCandleChartList.count()-GlobalVar::offsetLocal+m;
    if (n>GlobalVar::mCandleChartList.count()-1 or n<0)
        return;
    float temp=GlobalVar::mCandleChartList.at(n).pctChg;
    if (temp>0)
    {
        baseInfoData[0]->setPalette(GlobalVar::pRed);
        baseInfoData[1]->setPalette(GlobalVar::pRed);
    }
    else if(temp<0)
    {
        baseInfoData[0]->setPalette(GlobalVar::pGreen);
        baseInfoData[1]->setPalette(GlobalVar::pGreen);
    }
    else
    {
        baseInfoData[0]->setPalette(GlobalVar::pBlack);
        baseInfoData[1]->setPalette(GlobalVar::pBlack);
    }
    baseInfoData[0]->setText(QString::number(GlobalVar::mCandleChartList.at(n).close));
    baseInfoData[1]->setText(QString::number(GlobalVar::mCandleChartList.at(n).pctChg)+"%");
    baseInfoData[2]->setPalette(GlobalVar::pBlue);
    baseInfoData[3]->setPalette(GlobalVar::pBlue);
    baseInfoData[11]->setPalette(GlobalVar::pBlue);
    baseInfoData[2]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).turn)+"%");
    baseInfoData[3]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).amount));
    baseInfoData[11]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).vol));

    if (n==0)
        temp=GlobalVar::mCandleChartList.at(n).close/(1+GlobalVar::mCandleChartList.at(n).pctChg/100);
    else
        temp=GlobalVar::mCandleChartList.at(n-1).close;
    QList<float> t={GlobalVar::mCandleChartList.at(n).open,GlobalVar::mCandleChartList.at(n).high,
                    GlobalVar::mCandleChartList.at(n).low,temp};
    for (int i=0;i<3;++i)
    {
        if (t[i]>t[3])
            baseInfoData[i+8]->setPalette(GlobalVar::pRed);
        else if (t[i]<t[3])
            baseInfoData[i+8]->setPalette(GlobalVar::pGreen);
        else
            baseInfoData[i+8]->setPalette(GlobalVar::pBlack);
        baseInfoData[i+8]->setText(QString::number(t[i])+"("+QString::number((t[i]-temp)*100/temp,'f',2)+"%)");
    }
    rowTime->show();
    colPrice->show();
    if (mouseEvent->pos().ry()>drawChart.candleChart->height()*12/15)
        colPrice->hide();
    float price=drawChart.candleHighLowPoint[0]-(drawChart.candleHighLowPoint[0]-drawChart.candleHighLowPoint[1])*(mouseEvent->pos().ry()-KTOPHEIGHTEDGE)/(drawChart.candleChart->height()*12/15-2*KTOPHEIGHTEDGE);
    colPrice->setText(QString::number(price,'f',2));
    rowTime->setText(GlobalVar::mCandleChartList.at(n).time);
    colPrice->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-KWIDTHEDGE)*colPrice->width()/(drawChart.candleChart->width()-2*KWIDTHEDGE),mouseEvent->pos().ry()-25);
    rowTime->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-KWIDTHEDGE)*rowTime->width()/(drawChart.candleChart->width()-2*KWIDTHEDGE),drawChart.candleChart->height()*12/15);
}
