#include "mainwindow.h"
#include "qheaderview.h"
#include "ui_mainwindow.h"
#include "globalvar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initGlobalVar();
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
    for (int i=0;i<6;++i)
    {
        thread[i]->quit();
        thread[i]->wait();
    }
    delete threadTable;
    delete threadIndex;
    delete threadTimeShareTick;
    delete threadTimeShareChart;
    delete threadNewsReport;
    delete threadCandleChart;
    delete ui;
}

void MainWindow::initGlobalVar()
{
    GlobalVar::currentPath=QCoreApplication::applicationDirPath();
    GlobalVar::settings=new QSettings(GlobalVar::currentPath+"/config.ini",QSettings::IniFormat);
    GlobalVar::curCode=GlobalVar::settings->value("curCode").toString();
    GlobalVar::isSayNews=GlobalVar::settings->value("sayNews").toBool();
    GlobalVar::offsetEnd=GlobalVar::settings->value("offsetEnd").toInt();
    GlobalVar::pRed.setColor(QPalette::WindowText, Qt::red);
    GlobalVar::pGreen.setColor(QPalette::WindowText, QColor(0,191,0));
    GlobalVar::pBlack.setColor(QPalette::WindowText, Qt::black);
    GlobalVar::pBlue.setColor(QPalette::WindowText, Qt::blue);
}
void MainWindow::initThread()
{
    for (int i=0;i<6;++i)
        thread[i]=new QThread(this);

    threadTable=new ThreadTable;
    threadTable->moveToThread(thread[0]);
    connect(threadTable,SIGNAL(getTableDataFinished()),this,SLOT(reFlashTable()));
    connect(this,&MainWindow::startThreadTable,threadTable,&ThreadTable::getTableData);
    thread[0]->start();
    emit startThreadTable();

    threadIndex=new ThreadIndex;
    threadIndex->moveToThread(thread[1]);
    connect(threadIndex,SIGNAL(getIndexFinished()),this,SLOT(reFlashIndex()));
    connect(this,&MainWindow::startThreadIndex,threadIndex,&ThreadIndex::getAllIndex);
    thread[1]->start();
    emit startThreadIndex();

    threadTimeShareTick=new ThreadTimeShareTick;
    threadTimeShareTick->moveToThread(thread[2]);
    connect(threadTimeShareTick,&ThreadTimeShareTick::getTimeShareTickFinished,this,[=](){
        mTableStock.setTimeShareTickView();
    });
    connect(threadTimeShareTick,SIGNAL(getBuySellFinished()),this,SLOT(reFlashBuySellBaseInfo()));
    connect(this,&MainWindow::startThreadTimeShareTick,threadTimeShareTick,&ThreadTimeShareTick::getBuySellTimeShareTick);
    thread[2]->start();
    emit startThreadTimeShareTick();

    threadNewsReport=new ThreadNewsReport;
    threadNewsReport->moveToThread(thread[3]);
    connect(threadNewsReport,&ThreadNewsReport::getNewsFinished,this,[=](QString s){
        newsData->append(s);
});
    connect(this,&MainWindow::startThreadGetNews,threadNewsReport,&ThreadNewsReport::getNewsData);
    thread[3]->start();
    emit startThreadGetNews();

    threadTimeShareChart=new ThreadTimeShareChart;
    threadTimeShareChart->moveToThread(thread[4]);
    connect(threadTimeShareChart,&ThreadTimeShareChart::getTimeShareChartFinished,this,[=](){
        drawChart.timeShareChart->update();
    });
    connect(this,&MainWindow::startThreadTimeShareChart,threadTimeShareChart,&ThreadTimeShareChart::getAllTimeShareChart);
    thread[4]->start();
    emit startThreadTimeShareChart();

    threadCandleChart=new ThreadCandleChart;
    threadCandleChart->moveToThread(thread[5]);
    connect(threadCandleChart,&ThreadCandleChart::getCandleChartFinished,this,[=](){
        drawChart.candleChart->update();
    });
    connect(this,&MainWindow::startThreadCandleChart,threadCandleChart,&ThreadCandleChart::getAllCandleChart);
    thread[5]->start();
}
void MainWindow::initInterface()
{
    ui->setupUi(this);
    setWindowTitle("Stock");
    setWindowIcon(QIcon(":/new/pictures/logo.ico"));
    if (not GlobalVar::isSayNews)
        ui->newsReport->setChecked(true);
    QActionGroup *market = new QActionGroup(this);
    market->addAction(ui->HKMarket);
    market->addAction(ui->USMarket);
    market->addAction(ui->ZHMarket);
    ui->ZHMarket->setChecked(true);
//    market->addAction(ui->USzMarket);
    //所有股票界面
    ui->mainLayout->setContentsMargins(2,2,2,2);
    ui->mainLayout->addWidget(mTableStock.stockTableView);
    ui->mainLayout->addWidget(drawChart.candleChart);
    drawChart.candleChart->hide();


    //中间界面
    QVBoxLayout *mainLayout2 =new QVBoxLayout();
    mainLayout2->setSpacing(0);
    mainLayout2->setContentsMargins(0,0,0,0);
    mTableStock.risingSpeedView->setMaximumWidth(530);
    mTableStock.risingSpeedView->setMaximumHeight(454);
    mTableStock.myStockView->setMaximumWidth(530);
    mainLayout2->addWidget(mTableStock.risingSpeedView);
    mainLayout2->addWidget(mTableStock.myStockView);
    //右边界面
    rightBaseWindow=new QWidget(this);
    rightBaseWindow->setMaximumWidth(450);
    QVBoxLayout *rightLayout =new QVBoxLayout();
    rightBaseWindow->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0,0,0,0);
    //2
    rightFundWindow=new QWidget(this);
    rightFundWindow->setMaximumWidth(250);
    QVBoxLayout *rightFundLayout=new QVBoxLayout();
    rightFundWindow->setLayout(rightFundLayout);
//    rightFundLayout->setSpacing(0);
    rightFundLayout->setContentsMargins(20,30,20,2);

    ui->mainLayout->addLayout(mainLayout2);
    ui->mainLayout->addWidget(rightBaseWindow);
    ui->mainLayout->addWidget(rightFundWindow);
    rightFundWindow->hide();

    //右边界面布局
    QGridLayout *baseInfoLayout=new QGridLayout();
    baseInfoLayout->setSpacing(2);
    baseInfoLayout->setContentsMargins(10, 0, 0, 0);
    QGridLayout *buySellLayout=new QGridLayout();
    buySellLayout->setSpacing(2);
    buySellLayout->setContentsMargins(10, 5, 0, 0);
    newsData=new QTextBrowser(this);
    QHBoxLayout *freqAdjustLayout =new QHBoxLayout();
    freqAdjustLayout->setSpacing(0);

    rightLayout->addLayout(baseInfoLayout);
    rightLayout->addLayout(buySellLayout);
    rightLayout->addWidget(newsData);
    rightLayout->addLayout(freqAdjustLayout);
    QButtonGroup *freq=new QButtonGroup(this);
    QStringList periodAdjustName={"日线","周线","月线","不复权","前复权","后复权"};
    for (int i=0;i<6;++i)
    {
        periodAdjust[i]=new QRadioButton(periodAdjustName[i]);
        if (i<3)
            freq->addButton(periodAdjust[i]);
        freqAdjustLayout->addWidget(periodAdjust[i]);
    }
    periodAdjust[0]->setChecked(true);
    periodAdjust[3]->setChecked(true);

    drawChart.timeShareChart->setMinimumHeight(300);
    rightLayout->addWidget(drawChart.timeShareChart);

    initBaseInfoLayout(baseInfoLayout);
    initBuySellLayout(buySellLayout);
    //2
    periodBox->addItems({"即时", "3日排行", "5日排行", "10日排行", "20日排行"});
    northBox->addItems({"今日", "3日", "5日", "10日", "月", "季", "年"});
    singleStockBoard->addItems({"近一月", "近三月", "近半年", "近一年"});
    tradedetailBox->addItems({GlobalVar::curRecentWorkDay(false).toString("yyyy-MM-dd"),"近3日", "近5日", "近10日", "近30日"});
    openFundBox->addItems({"全部","股票型","混合型","债券型","指数型","QDII","LOF","FOF"});
    dateEdit->setCurrentSection(QDateEdit::DaySection);
    QStringList fundFlowName={"东方财富板块资金流","同花顺板块资金流","新高新低数量","股票热度、淘股吧",
                        "异常波动","昨日强势股票","北向资金买入","龙虎榜详情","个股上榜统计","开放式基金排行"};
    for (int i=0;i<10;++i)
    {
        if (i==1)
            rightFundLayout->addWidget(periodBox);
        else if(i==6)
            rightFundLayout->addWidget(dateEdit);
        else if(i==7)
            rightFundLayout->addWidget(northBox);
        else if(i==8)
            rightFundLayout->addWidget(tradedetailBox);
        else if(i==9)
            rightFundLayout->addWidget(singleStockBoard);
        fundFlow[i]=new QPushButton(fundFlowName[i]);
        rightFundLayout->addWidget(fundFlow[i]);
    }
    rightFundLayout->addWidget(openFundBox);
    QTextBrowser *instructionText=new QTextBrowser(this);
    instructionText->setStyleSheet("background-color:#F3F3F3;");
//    instructionText->setMaximumHeight(300);
    rightFundLayout->addWidget(instructionText);
    instructionText->append(GlobalVar::settings->value("Instruction").toString());
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
    mTableStock.stockTableView->verticalScrollBar()->installEventFilter(this);
    newsData->verticalScrollBar()->installEventFilter(this);
    newsData->setOpenExternalLinks(true);
    mTableStock.timeShareTickView->verticalScrollBar()->installEventFilter(this);
    drawChart.timeShareChart->installEventFilter(this);
    drawChart.candleChart->installEventFilter(this);
    drawChart.candleChart->setMouseTracking(true);

    mTableStock.stockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.risingSpeedView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.myStockView->setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    mTableStock.stockTableView->setFocusPolicy(Qt::NoFocus);
    mTableStock.risingSpeedView->setFocusPolicy(Qt::NoFocus);
    mTableStock.myStockView->setFocusPolicy(Qt::NoFocus);

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

    searchSmallWindow=new QWidget(this);
    searchSmallWindow->setWindowFlag(Qt::Popup);
    searchSmallWindow->setGeometry(1466, 645, 280, 350);
    QVBoxLayout *search =new QVBoxLayout();
    search->setContentsMargins(2, 2, 2, 2);
    searchSmallWindow->setLayout(search);
    search->addWidget(searchStock.searchCodeLine);
    search->addWidget(searchStock.matchCodeText);
    searchStock.searchCodeLine->installEventFilter(this);

    F10SmallWindow=new QWidget(this);
    F10SmallWindow->setWindowFlag(Qt::Popup);
    F10SmallWindow->move(200,100);
    QHBoxLayout *f10 =new QHBoxLayout();
    f10->setContentsMargins(2,2,2,2);
    F10SmallWindow->setLayout(f10);
    QWidget *navigation=new QWidget(F10SmallWindow);
    f10->addWidget(navigation);
    f10->addWidget(f10View.stockInfoView);
    QVBoxLayout *navi=new QVBoxLayout();
    navigation->setLayout(navi);
    QStringList F10InfoName={"热度概念排名","主要指标","经营分析","资产负债表","利润表","现金流量表"};
    for (int i=0;i<6;++i)
    {
        F10Info[i]=new QPushButton(F10InfoName[i]);
        navi->addWidget(F10Info[i]);
    }
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
    EPSLabel=new QLabel();
    EPSLabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑}");
    baseInfoLayout->addWidget(EPSLabel,7,2);
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

    mTableStock.timeShareTickView->setMinimumWidth(300);
    BuySellLayout->addWidget(mTableStock.timeShareTickView,0,3,11,3);
}
void MainWindow::initSignals()
{
    connect(mTableStock.stockTableView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        if (GlobalVar::WhichInterface==4)
        {
            if(ifCanClick==1)
                GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
            else if(ifCanClick==2)
                GlobalVar::curCode=mFundFlow.model->item(curRow,0)->text();
        }
        else
        {
            GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
            emit startThreadTimeShareTick();
            emit startThreadTimeShareChart();
        }
    });
    connect(mTableStock.risingSpeedView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mRisingSpeedList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mRisingSpeedList.at(curRow).name;
        emit startThreadTimeShareTick();
        emit startThreadTimeShareChart();
    });
    connect(mTableStock.myStockView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mMyStockList.at(curRow).name;
        emit startThreadTimeShareTick();
        emit startThreadTimeShareChart();
    });
    connect(ui->ZHMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->HKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USzMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(mTableStock.stockTableView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        if (GlobalVar::WhichInterface==4)
        {
            if (ifCanClick==0)
            {
                mFundFlow.getBoardStock(mFundFlow.FundFlowList.at(index.row())[0]);
                mTableStock.m_tableModel->setModelData(GlobalVar::mTableList);
                mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
                for (int i=0;i<16;++i)
                    mTableStock.stockTableView->setColumnWidth(i,90);
                ifCanClick=1;
            }
            else if(ifCanClick==1)
            {
                GlobalVar::isKState=true;
                int curRow=index.row();
                GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
                emit startThreadCandleChart(freq,adjustFlag,true);
                emit startThreadTimeShareTick();
                emit startThreadTimeShareChart();
                toInterFace("k");
            }
            else if(ifCanClick==2)
            {
                GlobalVar::isKState=true;
                int curRow=index.row();
                GlobalVar::curCode=mFundFlow.model->item(curRow,0)->text();
                mTableStock.stockTableView->setCurrentIndex(mFundFlow.model->index(curRow,0));
                emit startThreadCandleChart(freq,adjustFlag,true);
                emit startThreadTimeShareTick();
                emit startThreadTimeShareChart();
                toInterFace("k");
            }
            else
                QMessageBox::information(this,"提示","只能查看个股和东方板块",QMessageBox::Ok);
        }
        else
        {
            GlobalVar::isKState=true;
            emit startThreadCandleChart(freq,adjustFlag,true);
            toInterFace("k");
        }
    });
    connect(mTableStock.risingSpeedView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::isKState=true;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mRisingSpeedList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mRisingSpeedList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag,true);
        toInterFace("k");
    });
    connect(mTableStock.myStockView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::isKState=true;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mMyStockList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag,true);
        toInterFace("k");
    });
    // 信号发出，进行排序
    connect(mTableStock.stockTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
        //        bool is_asc;
        if (GlobalVar::WhichInterface==4 and ifCanClick!=1)
            return;
        if (GlobalVar::curSortNum!=logicalIndex)
        {
            GlobalVar::is_asc = false;
            GlobalVar::curSortNum=logicalIndex;
        }
        else
            GlobalVar::is_asc = not preSort;

        GlobalVar::sortByColumn(&GlobalVar::mTableList,logicalIndex,GlobalVar::is_asc);
        preSort=GlobalVar::is_asc;
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList);
        mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
        mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
    });
    connect(mTableStock.stockTableView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        if (GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==4)
            addRightMenu(1);
    });
    connect(mTableStock.risingSpeedView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        addRightMenu(2);
    });
    connect(mTableStock.myStockView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        delRightMenu();
    });
    for (int i = 2; i<=14; i=i+2)
    {
        QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(i));
        connect(bb,&QPushButton::clicked,this,[=](){
            GlobalVar::isKState=true;
            QString IndexCode[]={"1.000001","399001","399006","100.HSI","100.N225","100.KS11","1.000688","100.TWII","100.SENSEX","100.DJIA",
                                "100.SPX","100.NDX","100.SX5E","100.GDAXI","100.RTS","100.FTSE","100.FCHI","100.AS51","104.CN00Y","103.YM00Y"};
            int n;
            if (isAsia)
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
            resetKParameter();
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
            toInterFace("k");
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
    connect(ui->fundFlow,&QAction::triggered,this,&MainWindow::dealWithFundFlow);
    connect(&searchStock,SIGNAL(showSearch()),this,SLOT(showSearchResult()));
    connect(ui->DLStockInfo,&QAction::triggered,this,&MainWindow::downStockIndexPlateInfo);
    for (int i=0;i<6;++i)
        connect(periodAdjust[i],&QRadioButton::clicked,this,&MainWindow::initFlag);

    connect(F10Info[0],&QPushButton::clicked,this,[=](){
        setF3Window();
    });
    connect(ui->actionF3,&QAction::triggered,this,[=](){
        setF3Window();
    });
    connect(F10Info[1],&QPushButton::clicked,this,[=](){
        setF10Window();
    });
    connect(ui->actionF10,&QAction::triggered,this,[=](){
        setF10Window();
    });
    connect(F10Info[2],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1470,700);
        f10View.dealWithBusinessAnalysis();
    });
    connect(F10Info[3],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1330,700);
        f10View.dealWithAssetLiability();
    });
    connect(F10Info[4],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1430,700);
        f10View.dealWithIncome();
    });
    connect(F10Info[5],&QPushButton::clicked,this,[=](){
        F10SmallWindow->setFixedSize(1430,700);
        f10View.dealWithCashFlow();
    });
    for (int i=0;i<10;++i)
        connect(fundFlow[i],&QPushButton::clicked,this,&MainWindow::toFundFlow);
    connect(f10View.stockInfoView,&QTableView::doubleClicked,this,[=](const QModelIndex &index){
        GlobalVar::curBoard=f10View.model->item(index.row(),3)->text();
        GlobalVar::isBoard=true;
        searchStock.getBoardData();
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList);
        mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
        mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
        toInterFace("main");
        mTableStock.risingSpeedView->show();
        mTableStock.myStockView->show();
    });
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
//        GlobalVar::settings->setValue("curName",GlobalVar::curName);
        GlobalVar::settings->sync();
    }
}
void MainWindow::initFlag()
{
    if (sender()==periodAdjust[0])
        freq="101";
    else if (sender()==periodAdjust[1])
        freq="102";
    else if (sender()==periodAdjust[2])
        freq="103";
    else if (sender()==periodAdjust[3])
        adjustFlag="0";
    else if (sender()==periodAdjust[4])
        adjustFlag="1";
    else if (sender()==periodAdjust[5])
        adjustFlag="2";
    if (GlobalVar::isKState)
    {
        preCode="";
        emit startThreadCandleChart(freq,adjustFlag,true);
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mTableStock.stockTableView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();//获得鼠标滚轮的滚动距离para，para<0向下滚动，>0向上滚动
        int tempStep=mTableStock.stockTableView->verticalScrollBar()->value();
        int curIndex=mTableStock.stockTableView->currentIndex().row();
        int row=int(mTableStock.stockTableView->height()/22.0+0.5);
        if (para<0)
        {
            //向下滚动，设定鼠标滚轮每滚动一个单位，滑块就移动20个单位
            mTableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>GlobalVar::mTableList.count()-row)
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
            else
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curIndex+row,0));
        }
        else
        {
            //向上滚动
            mTableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curIndex-row,0));
            else
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(GlobalVar::mTableList.count()-1,0));
        }
//        qDebug()<<mTableStock.stockTableView->height();
        return true;
    }
    else if (obj==drawChart.timeShareChart and event->type() == QEvent::Paint)
    {
        drawChart.drawTimeShareChart();
        return true;
    }
    else if(obj==drawChart.candleChart and event->type()==QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        flashOldCandleInfo(mouseEvent);
        return true;
    }
    else if(obj==drawChart.candleChart and event->type()==QEvent::Paint)
    {
        drawChart.drawCandleChart();
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
    else if(obj==mTableStock.timeShareTickView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=mTableStock.timeShareTickView->verticalScrollBar()->value();
        if (para<0)
            mTableStock.timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep+13);
        else
            mTableStock.timeShareTickView->verticalScrollBar()->setSliderPosition(tempStep-13);
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
        if (GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==4)
            addRightMenu(3);
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QKeySequence key=event->key();
    if (key==Qt::Key_Escape)
    {
        resetKParameter();
        if (GlobalVar::isKState)
        {
            GlobalVar::isKState=false;
            if (GlobalVar::WhichInterface==1)
            {
                mTableStock.risingSpeedView->show();
                mTableStock.myStockView->show();
            }
            else if(GlobalVar::WhichInterface==4)
            {
                rightFundWindow->show();
                rightBaseWindow->hide();
            }
            drawChart.candleChart->hide();
            mTableStock.stockTableView->show();
//            mTableStock.stockTableView->setFocus();
        }
    }
    else if (key==Qt::Key_PageDown)
    {
        if (GlobalVar::isKState)
        {
            resetKParameter();
            if (ifCanClick==2)
            {
                int curRow=mTableStock.stockTableView->currentIndex().row()+1;
                if (curRow>mFundFlow.model->rowCount()-1)
                    curRow=0;
                GlobalVar::curCode=mFundFlow.model->item(curRow,0)->text();
                mTableStock.stockTableView->setCurrentIndex(mFundFlow.model->index(curRow,0));
            }
            else
            {
                int curRow=mTableStock.stockTableView->currentIndex().row()+1;
                if (curRow>GlobalVar::mTableList.count()-1)
                    curRow=0;
                GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
            }
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
    }
    else if (key==Qt::Key_PageUp)
    {
        if (GlobalVar::isKState)
        {
            resetKParameter();
            if (ifCanClick==2)
            {
                int curRow=mTableStock.stockTableView->currentIndex().row()-1;
                if (curRow<0)
                    curRow=mFundFlow.model->rowCount()-1;
                GlobalVar::curCode=mFundFlow.model->item(curRow,0)->text();
                mTableStock.stockTableView->setCurrentIndex(mFundFlow.model->index(curRow,0));
            }
            else
            {
                int curRow=mTableStock.stockTableView->currentIndex().row()-1;
                if (curRow<0)
                    curRow=GlobalVar::mTableList.count()-1;
                GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
            }
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
    }
    else if (key==Qt::Key_Enter or key==Qt::Key_Return)
    {
        if (GlobalVar::isKState or GlobalVar::WhichInterface==4)
            return;
        GlobalVar::isKState=true;
        emit startThreadCandleChart(freq,adjustFlag,true);
        toInterFace("k");
    }
    else if ((key >= 48 and key <= 57) or (key >= 65 and key <= 90))
    {
//        qDebug()<<event->text();
//        searchCode+=event->text();
        if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5)
            return;
        searchSmallWindow->show();
        searchStock.searchCodeLine->setText(event->text());
        searchStock.searchCodeLine->setFocus();
        searchStock.matchCodeText->moveCursor(QTextCursor::Start);
    }
    else if (key==Qt::Key_Up)
    {
        if (GlobalVar::isKState)
        {
            int temp=GlobalVar::KRange;
            GlobalVar::KRange=GlobalVar::KRange*0.8;

            if (GlobalVar::KRange<50)
            {
                GlobalVar::KRange=50;
                return;
            }
            GlobalVar::offsetLocal=GlobalVar::offsetLocal-temp+GlobalVar::KRange;
//            if (GlobalVar::offsetLocal<GlobalVar::KRange)
//                GlobalVar::offsetLocal=GlobalVar::KRange;

            drawChart.candleChart->update();
        }
    }
    else if (key==Qt::Key_Down)
    {
        if (GlobalVar::isKState)
        {
            GlobalVar::offsetLocal=GlobalVar::offsetLocal+GlobalVar::KRange*0.1;
            GlobalVar::KRange=GlobalVar::KRange*1.2;
            if (GlobalVar::mCandleChartList.count()<GlobalVar::offsetLocal)
                GlobalVar::offsetLocal=GlobalVar::mCandleChartList.count();
            if (GlobalVar::KRange>GlobalVar::offsetLocal)
                GlobalVar::KRange=GlobalVar::offsetLocal;
            if (preCode!=GlobalVar::curCode)
            {
                preCode=GlobalVar::curCode;
                emit startThreadCandleChart(freq,adjustFlag,false);
            }
            drawChart.candleChart->update();
        }
    }
    else if (key==Qt::Key_Left)
    {
        if (GlobalVar::isKState)
        {

            GlobalVar::offsetLocal=GlobalVar::offsetLocal+GlobalVar::KRange*0.2;
            if (GlobalVar::mCandleChartList.count()<GlobalVar::offsetLocal)
                GlobalVar::offsetLocal=GlobalVar::mCandleChartList.count();
            if (preCode!=GlobalVar::curCode)
            {
                preCode=GlobalVar::curCode;
                emit startThreadCandleChart(freq,adjustFlag,false);
            }
            drawChart.candleChart->update();
        }
    }
    else if (key==Qt::Key_Right)
    {
        if (GlobalVar::isKState)
        {
            GlobalVar::offsetLocal=GlobalVar::offsetLocal-GlobalVar::KRange*0.2;
            if (GlobalVar::offsetLocal<GlobalVar::KRange)
                GlobalVar::offsetLocal=GlobalVar::KRange;
            GlobalVar::offsetEnd=0;
            drawChart.candleChart->update();
//            GlobalVar::offsetEnd=OFFSET;
        }
    }
    else if (key==Qt::Key_F3)
        setF3Window();
    else if (key==Qt::Key_F10)
        setF10Window();
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (GlobalVar::isKState)
    {
        resetKParameter();
        int curRow;
        int counts=0;
        if (ifCanClick==2)
            counts=mFundFlow.model->rowCount()-1;
        else
            counts=GlobalVar::mTableList.count()-1;
        if (event->angleDelta().y()<0)
        {
            curRow=mTableStock.stockTableView->currentIndex().row()+1;
            if (curRow>counts)
                curRow=0;
        }
        else
        {
            curRow=mTableStock.stockTableView->currentIndex().row()-1;
            if (curRow<0)
                curRow=counts;
        }
        if (ifCanClick==2)
        {
            GlobalVar::curCode=mFundFlow.model->item(curRow,0)->text();
            mTableStock.stockTableView->setCurrentIndex(mFundFlow.model->index(curRow,0));
        }
        else
        {
            GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
            mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
        }
        //            qDebug()<<GlobalVar::curCode;
        emit startThreadCandleChart(freq,adjustFlag,true);
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
        isAsia=true;
        ifCanClick=1;
        mTableStock.risingSpeedView->show();
        mTableStock.myStockView->show();
//        ui->HKMarket->setChecked(false);
//        ui->USMarket->setChecked(false);
//        ui->ZHMarket->setChecked(true);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode=GlobalVar::settings->value("curCode").toString();
        GlobalVar::curName=GlobalVar::settings->value("curName").toString();
    }
    else if (obj->objectName()=="HKMarket")
    {
        saveCode();
        ifCanClick=-1;
        GlobalVar::WhichInterface=2;
        isAsia=true;
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
//        ui->HKMarket->setChecked(true);
//        ui->USMarket->setChecked(false);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="00700";
        GlobalVar::curName="腾讯控股";
    }
    else if (obj->objectName()=="USMarket")
    {
        saveCode();
        ifCanClick=-1;
        GlobalVar::WhichInterface=5;
        isAsia=false;
        GlobalVar::isUsZhStock=false;
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
//        ui->HKMarket->setChecked(false);
//        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="105.MSFT";
        GlobalVar::curName="微软";
    }
    else if (obj->objectName()=="USzMarket")
    {
        saveCode();
        ifCanClick=-1;
        GlobalVar::WhichInterface=5;
        isAsia=false;
        GlobalVar::isUsZhStock=true;
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
//        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(true);
        GlobalVar::curCode="106.BABA";
        GlobalVar::curName="阿里巴巴";
    }
    GlobalVar::isBoard=false;
    GlobalVar::isKState=false;
    resetKParameter();
    mTableStock.stockTableView->show();
    drawChart.candleChart->hide();
    rightBaseWindow->show();
    rightFundWindow->hide();
    emit startThreadTable();
    emit startThreadTimeShareChart();
    emit startThreadTimeShareTick();
    emit startThreadIndex();
}
void MainWindow::addRightMenu(int num)
{
    StockInfo info;
    QString code;
    if (num==1)
    {
        if (ifCanClick==1)
            code=GlobalVar::mTableList.at(mTableStock.stockTableView->currentIndex().row()).code;
        else if (ifCanClick==2)
            code=mFundFlow.model->item(mTableStock.stockTableView->currentIndex().row(),0)->text();
        else
            return;
    }
    else if(num==2)
        code=GlobalVar::mRisingSpeedList.at(mTableStock.risingSpeedView->currentIndex().row()).code;
    else if(num==3)
    {
        code=GlobalVar::curCode;
        if (code.left(1)=="1" or code.left(3)=="399")
            return;
    }
    int l = 0;
    int r = GlobalVar::mTableListCopy.count() - 1;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        if (GlobalVar::mTableListCopy.at(mid).code == code)
        {
            info=GlobalVar::mTableListCopy.at(mid);
            break;
        }
        else if (GlobalVar::mTableListCopy.at(mid).code > code)
            r = mid - 1;
        else
            l = mid + 1;
    }
    QMenu *menu=new QMenu();
    QAction *act=new QAction("加入自选");
    menu->addAction(act);
    menu->popup(QCursor::pos());
    connect(act,&QAction::triggered,this,[info, this](){
        for (int i=0;i<GlobalVar::mMyStockList.count();++i)
        {
            if (info.code==GlobalVar::mMyStockList.at(i).code)
                return;
        }
        GlobalVar::mMyStockList.append(info);
        int curIndex=mTableStock.myStockView->currentIndex().row();
        mTableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList);
        mTableStock.myStockView->setModel(mTableStock.m_myStockModel);
        mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curIndex,0));
        saveMyStock();
    });

}
void MainWindow::delRightMenu()
{
    QMenu *menu=new QMenu();
    QAction *act=new QAction("删除自选");
    menu->addAction(act);
    menu->popup(QCursor::pos());
    connect(act,&QAction::triggered,this,[=](){
        GlobalVar::mMyStockList.removeAt(mTableStock.myStockView->currentIndex().row());
        mTableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList);
        mTableStock.myStockView->setModel(mTableStock.m_myStockModel);
        saveMyStock();
    });
}
void MainWindow::showSearchResult()
{
    if (GlobalVar::isBoard)
    {
        resetKParameter();
        searchSmallWindow->hide();
        GlobalVar::WhichInterface=1;
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList);
        mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
        mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
        emit startThreadTable();
        toInterFace("main");
        mTableStock.risingSpeedView->show();
        mTableStock.myStockView->show();
    }
    else
    {
        GlobalVar::isKState=true;
        resetKParameter();
        searchSmallWindow->hide();
        emit startThreadCandleChart(freq,adjustFlag,true);
        emit startThreadTimeShareChart();
        emit startThreadTimeShareTick();
        toInterFace("k");
    }
}
void MainWindow::downStockIndexPlateInfo()
{
    QWidget *promptWindow=new QWidget(this);
    QTextEdit *promptText=new QTextEdit(promptWindow);
    promptText->setStyleSheet("background-color: rgb(211, 211, 211);");
    promptWindow->setWindowFlag(Qt::Popup);
    promptWindow->setGeometry(850, 400, 300, 150);
    QVBoxLayout *Vlay=new QVBoxLayout(promptWindow);
    promptWindow->setLayout(Vlay);
    Vlay->addWidget(promptText);
    promptWindow->show();
    promptText->append("开始下载指数、板块、个股信息...请稍等");
    requestsToCsv.dealWithAllList();
    promptText->append("指数、板块、个股信息处理完毕");
//    ui->DLStockInfo->setDisabled(true);
}
void MainWindow::resetKParameter()
{
    GlobalVar::offsetEnd=GlobalVar::settings->value("offsetEnd").toInt();
    GlobalVar::KRange=KRANGE;
    GlobalVar::offsetLocal=GlobalVar::KRange;
}
void MainWindow::dealWithFundFlow()
{
    int n=periodBox->currentIndex();
    if (n==1 or n==4)
    {
        QMessageBox::information(this,"提示","东方只能查看1日、5日和10日",QMessageBox::Ok);
        return;
    }
    dateEdit->setDate(GlobalVar::curRecentWorkDay(false).date());
    tradedetailBox->setItemText(0,GlobalVar::curRecentWorkDay(false).toString("yyyy-MM-dd"));

    GlobalVar::isKState=false;
    ifCanClick=0;
    GlobalVar::WhichInterface=4;
    toInterFace("fund");
    int days[]={1,3,5,10,20};
    mFundFlow.getEastPlateFundFlow(days[n]);
    mTableStock.stockTableView->setModel(mFundFlow.model);
    mTableStock.stockTableView->setColumnWidth(0,140);
    mTableStock.stockTableView->setColumnWidth(5,120);
    mTableStock.stockTableView->setColumnWidth(12,160);
    for (int i=1;i<5;++i)
        mTableStock.stockTableView->setColumnWidth(i,100);
    for (int i=6;i<12;++i)
        mTableStock.stockTableView->setColumnWidth(i,100);
}
//交易日每5秒刷新一次数据
void MainWindow::tradingTimeRunThread()
{
//    int a = timeCount % 10;
    //每1秒，A股交易时段刷新个股买卖及每笔成交
    QDateTime curTime=QDateTime::currentDateTime();
    if (timeCount%2==1)
    {
        if (GlobalVar::WhichInterface==1 and GlobalVar::isZhMarketDay(curTime))
            if (GlobalVar::curCode.left(1)!="1")
                emit startThreadTimeShareTick();
    }
    //每5秒
    else if (timeCount==8 or timeCount==18)
    {
        //A股交易时段实时刷新所有股票，及分时图
        if (GlobalVar::WhichInterface==1 and GlobalVar::isZhMarketDay(curTime))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            if (GlobalVar::isBoard)
            {
                searchStock.getBoardData();
//                mTableStock.m_tableModel->setModelData(GlobalVar::mTableList);
//                mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
//                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
            }
            emit startThreadTable();
            emit startThreadTimeShareChart();

        }
        //美股港股交易时段实时刷新所有股票，及分时图、分时成交
        else if ((GlobalVar::WhichInterface==5 and GlobalVar::isUSMarketDay(curTime)) or
                 (GlobalVar::WhichInterface==2 and GlobalVar::isHKMarketDay(curTime)))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            emit startThreadTable();
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
        }
        //A股交易时段实时刷新k线图
        else if (GlobalVar::isKState and GlobalVar::isZhMarketDay(curTime))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            emit startThreadCandleChart(freq,adjustFlag,true);
        }
        //标注非交易时段
        else
            circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);

        //工作日

        if (GlobalVar::isWorkDay(curTime))
        {
            //更新全球指数
            emit startThreadIndex();
            //每天9点,更新A股指数板块股票信息
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
    }
    //每10秒获取最新新闻信息
    else if (timeCount==20)
    {
        emit startThreadGetNews();
        timeCount=0;
    }
    timeCount+=1;
//    qDebug()<<timeCount;
}
void MainWindow::reFlashTable()
{
    mTableStock.setTableView();
    baseInfoData[7]->setText(QString::number(GlobalVar::upNums));
    baseInfoData[15]->setText(QString::number(GlobalVar::downNums));
}
void MainWindow::reFlashIndex()
{
    changeInTurn=not changeInTurn;
//    qDebug()<<ui->statusBar->children();
    int n;
    for (int i = 2; i<=12; i=i+2)
    {
        QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(i));
        QLabel *bl = (QLabel *)(ui->statusBar->children().at(i+1));
        if (isAsia)
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
    stockName->setText(GlobalVar::curName);
    EPSLabel->setText(GlobalVar::EPSReportDate);
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
void MainWindow::setF3Window()
{
    F10SmallWindow->setFixedSize(675,500);
    F10SmallWindow->show();
    f10View.dealWithHotRank();
}
void MainWindow::setF10Window()
{
    F10SmallWindow->setFixedSize(1275,700);
    F10SmallWindow->show();
    f10View.dealWithMainIndex();
}
void MainWindow::toInterFace(QString which)
{
    if (which=="main")
    {
        rightFundWindow->hide();
        drawChart.candleChart->hide();
        rightBaseWindow->show();
        mTableStock.stockTableView->show();
//        mTableStock.stockTableView->setFocus();
    }
    else if (which=="k")
    {
        GlobalVar::isKState=true;
        mTableStock.stockTableView->hide();
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
        rightFundWindow->hide();
        drawChart.candleChart->show();
        rightBaseWindow->show();
        rightBaseWindow->setFocus();
    }
    else if (which=="fund")
    {
        GlobalVar::WhichInterface=4;
        GlobalVar::isKState=false;
        rightBaseWindow->hide();
        drawChart.candleChart->hide();
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
        rightFundWindow->show();
        mTableStock.stockTableView->show();
//        mTableStock.stockTableView->setFocus();
    }
    else
        QMessageBox::information(this,"提示", "界面转换错误", QMessageBox::Ok);
}
void MainWindow::toFundFlow()
{
    if (sender()==fundFlow[0])
        dealWithFundFlow();
    else if((sender()==fundFlow[1]))
    {
        ifCanClick=-1;
        mFundFlow.getRoyalFlushFundFlow();
        mTableStock.stockTableView->setModel(mFundFlow.model);
    }
    else if((sender()==fundFlow[2]))
    {
        ifCanClick=-1;
        mFundFlow.getIntervalHighLow();
        mTableStock.stockTableView->setModel(mFundFlow.model);
    }
    else if((sender()==fundFlow[3]))
    {
        ifCanClick=2;
        mFundFlow.getStockHot();
        mTableStock.stockTableView->setModel(mFundFlow.model);
    }
    else if((sender()==fundFlow[4]))
    {
        ifCanClick=-1;
        mFundFlow.getNotNormalStock();
        mTableStock.stockTableView->setModel(mFundFlow.model);
    }
    else if((sender()==fundFlow[5]))
    {
        ifCanClick=2;
        mFundFlow.getStockPoolStrong(dateEdit->date().toString("yyyyMMdd"));
        mTableStock.stockTableView->setModel(mFundFlow.model);
        for (int i=0;i<15;++i)
            mTableStock.stockTableView->setColumnWidth(i,90);
    }
    else if((sender()==fundFlow[6]))
    {
        ifCanClick=2;
        QString days[]={"1","3","5","10","M","Q","Y"};
        mFundFlow.getNorthFundFlow(days[northBox->currentIndex()]);
        mTableStock.stockTableView->setModel(mFundFlow.model);
        for (int i=0;i<14;++i)
            mTableStock.stockTableView->setColumnWidth(i,90);
        mTableStock.stockTableView->setColumnWidth(6,120);
        mTableStock.stockTableView->setColumnWidth(7,120);
        mTableStock.stockTableView->setColumnWidth(10,120);
        mTableStock.stockTableView->setColumnWidth(11,120);
        mTableStock.stockTableView->setColumnWidth(12,120);
    }
    else if((sender()==fundFlow[7]))
    {
        ifCanClick=2;
        int nums[]={1,3,5,10,30};
        int pages[]={1,1,1,2,4};
        mFundFlow.getDragonTigerList(nums[tradedetailBox->currentIndex()],pages[tradedetailBox->currentIndex()]);
        mTableStock.stockTableView->setModel(mFundFlow.model);
        mTableStock.stockTableView->setColumnWidth(0,60);
        mTableStock.stockTableView->setColumnWidth(1,75);
        mTableStock.stockTableView->setColumnWidth(2,210);
        for (int i=3;i<9;++i)
            mTableStock.stockTableView->setColumnWidth(i,75);
        mTableStock.stockTableView->setColumnWidth(9,90);
        mTableStock.stockTableView->setColumnWidth(10,100);
        mTableStock.stockTableView->setColumnWidth(11,100);
        mTableStock.stockTableView->setColumnWidth(12,65);
        mTableStock.stockTableView->setColumnWidth(13,85);
        mTableStock.stockTableView->setColumnWidth(14,300);
    }
    else if((sender()==fundFlow[8]))
    {
        ifCanClick=2;
        QString nums[]={"01","02","03","04"};
        int pages[]={2,3,4,7};
        mFundFlow.countSingleStockBoard(nums[singleStockBoard->currentIndex()],pages[singleStockBoard->currentIndex()]);
        mTableStock.stockTableView->setModel(mFundFlow.model);
        mTableStock.stockTableView->setColumnWidth(0,65);
        mTableStock.stockTableView->setColumnWidth(1,80);
        mTableStock.stockTableView->setColumnWidth(2,100);
        mTableStock.stockTableView->setColumnWidth(3,65);
        mTableStock.stockTableView->setColumnWidth(4,65);
        mTableStock.stockTableView->setColumnWidth(5,65);
        for (int i=6;i<19;++i)
            mTableStock.stockTableView->setColumnWidth(i,90);
    }
    else if((sender()==fundFlow[9]))
    {
        ifCanClick=-1;
        QString ft[]={"all","gp","hh","zq","zs","qdii","lof","fof"};
        QString sc[]={"zzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf", "6yzf"};
        mFundFlow.openFundRank(ft[openFundBox->currentIndex()],sc[openFundBox->currentIndex()]);
        mTableStock.stockTableView->setModel(mFundFlow.model);
        mTableStock.stockTableView->setColumnWidth(0,65);
        mTableStock.stockTableView->setColumnWidth(1,280);
        mTableStock.stockTableView->setColumnWidth(2,100);
        for (int i=3;i<16;++i)
            mTableStock.stockTableView->setColumnWidth(i,90);
    }
}
