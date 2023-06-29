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
    for (int i=0;i<5;++i)
        GlobalVar::areaFlag[i]=true;
    GlobalVar::pRed.setColor(QPalette::WindowText, Qt::red);
    GlobalVar::pGreen.setColor(QPalette::WindowText, QColor(0,191,0));
    GlobalVar::pBlack.setColor(QPalette::WindowText, Qt::black);
    GlobalVar::pBlue.setColor(QPalette::WindowText, Qt::blue);
    GlobalVar::pWhite.setColor(QPalette::WindowText, Qt::white);
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
        QWidget *pActiveWindow = QApplication::activeWindow();
        MainWindow *pMainWindow = dynamic_cast<MainWindow*>(pActiveWindow);
        if(pMainWindow && pMainWindow == this)
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
    if (GlobalVar::settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
        ui->setVacation->setEnabled(false);
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
    tradedetailBox->addItems({GlobalVar::curRecentWorkDay(1).toString("yyyy-MM-dd"),"近3日", "近5日", "近10日", "近30日"});
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
    newsData->setOpenExternalLinks(true);
    drawChart.hisTimeShareChart->setParent(this);

    mTableStock.stockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.risingSpeedView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.myStockView->setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    mTableStock.stockTableView->setFocusPolicy(Qt::NoFocus);
    mTableStock.risingSpeedView->setFocusPolicy(Qt::NoFocus);
    mTableStock.myStockView->setFocusPolicy(Qt::NoFocus);

    searchSmallWindow=new QWidget(this);
    searchSmallWindow->setWindowFlag(Qt::Popup);
    searchSmallWindow->setGeometry(1466, 645, 300, 350);
    QVBoxLayout *search =new QVBoxLayout();
    search->setContentsMargins(2, 2, 2, 2);
    searchSmallWindow->setLayout(search);
    search->addWidget(searchStock.searchCodeLine);
    search->addWidget(searchStock.matchCodeText);

    F10SmallWindow=new QWidget(this);
    QWidget *F10Title=new QWidget(this);
    F10SmallWindow->setWindowFlag(Qt::Popup);
    F10SmallWindow->move(200,100);
    QVBoxLayout *f10Main=new QVBoxLayout();
    F10SmallWindow->setLayout(f10Main);
    QHBoxLayout *f10 =new QHBoxLayout();
    QHBoxLayout *titleLayout=new QHBoxLayout();

    QPushButton *close=new QPushButton();
    connect(close,&QPushButton::clicked,this,[=](){F10SmallWindow->close();});
    F10Title->setLayout(titleLayout);
    F10Title->setMaximumHeight(TITLEHEIGHT);
//    f10Title->setStyleSheet("background-color:rgb(153, 204, 255);");
    fTitle->setStyleSheet("QLabel{font:bold 18px;font-family:微软雅黑;color:blue}");

    f10Main->addWidget(F10Title);
    f10Main->addLayout(f10);
    f10Main->setContentsMargins(0,0,0,0);

    QIcon myicon;
    myicon.addFile(tr(":/new/pictures/close.png"));
    close->setIcon(myicon);
    close->setIconSize(QSize(20,20));
    close->setMaximumSize(QSize(30,30));
    titleLayout->addWidget(fTitle);
    titleLayout->addWidget(close);

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

    QString lName[]={"现价", "涨幅","换手", "成交额", "总股本", "总市值", "市净率", "上涨总数",
                       "今开", "最高", "最低", "成交量(手)", "流通股", "", "", "下跌总数"};

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
    PELabel=new QLabel();
    PELabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑}");
    baseInfoLayout->addWidget(EPSLabel,7,2);
    baseInfoLayout->addWidget(PELabel,8,2);
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
    line->setStyleSheet("QFrame{background:yellow;min-height:2px;border-top:2px dotted}");
//    line->setFrameShape(QFrame::HLine);
//    line->setFrameShadow(QFrame::Sunken);
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
        isLookMyStock=true;
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
            if (not drawChart.hisTimeShareChart->isHidden())
                drawChart.hisTimeShareChart->close();
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
    connect(ui->DLAllStockK,&QAction::triggered,this,[=](){
        requestsToCsv.downloadAllStockK();
    });
    connect(ui->DLStockInfo,&QAction::triggered,this,[=](){
        requestsToCsv.downStockIndexPlateInfo();
    });
    for (int i=0;i<6;++i)
        connect(periodAdjust[i],&QRadioButton::clicked,this,&MainWindow::initFlag);

    connect(F10Info[0],&QPushButton::clicked,this,[=](){
        toInterFace("f3");
    });
    connect(ui->actionF3,&QAction::triggered,this,[=](){
        toInterFace("f3");
    });
    connect(F10Info[1],&QPushButton::clicked,this,[=](){
        toInterFace("f10");
    });
    connect(ui->actionF10,&QAction::triggered,this,[=](){
        toInterFace("f10");
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
    connect(ui->pickStock,&QAction::triggered,this,[=](){
        mPickStock->PickStockInterface();
    });
    connect(mPickStock,&JSPickStock::updateTableList,this,[=](){
        mTableStock.setTableView();
    });
    connect(ui->formula,&QAction::triggered,this,[](){
        QDialog *formulaDes=new QDialog();
        formulaDes->setWindowTitle("编写公式说明");
        formulaDes->setGeometry(450, 200, 1000, 700);
        formulaDes->setAttribute(Qt::WA_DeleteOnClose);
        QTextBrowser *des=new QTextBrowser(formulaDes);
        des->setMinimumHeight(380);
        des->setStyleSheet("QTextBrowser{font:bold 22px;font:bold}");
        QVBoxLayout *mainLayout =new QVBoxLayout();
        mainLayout->setContentsMargins(2,0,2,2);
        mainLayout->setSpacing(0);
        formulaDes->setLayout(mainLayout);
        mainLayout->addWidget(des);
        QFile file(GlobalVar::currentPath+"/list/formulades.txt");
        if (file.open(QFile::ReadOnly))
        {
            QTextCodec *codec = QTextCodec::codecForName("utf-8");
            des->setText(codec->toUnicode(file.readAll()));
            int post=0;
            QString s=des->toPlainText();
            QTextCharFormat fmt;
            fmt.setForeground(QColor("red"));
            while((post=s.indexOf("(",post))!=-1)
            {
                QString t=s.mid(post-1,1);
                if (t>='A' && t<='Z')
                {
                    QTextCursor cursor = des->textCursor();
                    int p=s.indexOf(")",post)+1;
                    cursor.setPosition(post-1,QTextCursor::MoveAnchor);
                    cursor.setPosition(p,QTextCursor::KeepAnchor);
                    cursor.mergeCharFormat(fmt);
                }
                post+=1;
            }
        }
//        des->setCursor();
        QTextBrowser *d=new QTextBrowser(formulaDes);
        mainLayout->addWidget(d);
        d->insertHtml("<img src=:/new/pictures/des.jpg/>");
        formulaDes->show();
        file.close();
    });
    connect(ui->setVacation,&QAction::triggered,this,[=](){
        mFundFlow.getVacation();
        if (GlobalVar::settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
            ui->setVacation->setEnabled(false);
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
    else if (obj==drawChart.timeShareChart)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter painter(drawChart.timeShareChart);
            drawChart.drawTimeShareChart(painter);
            painter.end();
        }
        else if(event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            //        float price=GlobalVar::timeShareHighLowPoint[0]-(GlobalVar::timeShareHighLowPoint[0]-GlobalVar::timeShareHighLowPoint[1])*(mouseEvent->pos().ry()-TOPHEIGHTEDGE)/(drawChart.timeShareChart->height()*12/15-2*TOPHEIGHTEDGE);
            //        timeSharePrice->setText(QString::number((1+price/100)*GlobalVar::preClose,'f',2)+"("+QString::number(price,'f',2)+")%");
            int n=int(mouseEvent->pos().rx()*GlobalVar::trendsTotal/(drawChart.timeShareChart->width()-2*WIDTHEDGE))-1;
            if (n<0 or n>GlobalVar::mTimeShareChartList.size()-1 or
                mouseEvent->pos().ry()<TOPHEIGHTEDGE or
                mouseEvent->pos().ry()>drawChart.timeShareChart->height()-BOTTOMHEIGHTEDGE)
            {
                drawChart.timeSharePrice->hide();
                drawChart.timeShareVol->hide();
                drawChart.timeShareTime->hide();
                drawChart.vLine.hide();
                drawChart.hLine.hide();
                return false;
            }
            drawChart.vLine.setStyleSheet("QLabel{border:2px dotted white;}");
            drawChart.vLine.resize(1,drawChart.timeShareChart->height()-BOTTOMHEIGHTEDGE);
            drawChart.hLine.setStyleSheet("QLabel{border:2px dotted white;}");
            drawChart.hLine.resize(drawChart.timeShareChart->width()-2*WIDTHEDGE,1);
            float p=GlobalVar::mTimeShareChartList.at(n).price;
            if (p>0)
                drawChart.timeSharePrice->setStyleSheet("color:red;font:bold;font-size:18px");
            else
                drawChart.timeSharePrice->setStyleSheet("color:white;font:bold;font-size:18px");
            float price=(1+p/100)*GlobalVar::preClose;
            int x=(drawChart.timeShareChart->width()-2*WIDTHEDGE)*n/GlobalVar::trendsTotal+WIDTHEDGE;
            int y=(GlobalVar::timeShareHighLowPoint[0]-p)*(drawChart.timeShareChart->height()*12/15-2*TOPHEIGHTEDGE)/(GlobalVar::timeShareHighLowPoint[0]-GlobalVar::timeShareHighLowPoint[1])+TOPHEIGHTEDGE;
            //        qDebug()<<x<<y;
            drawChart.timeSharePrice->setText(QString::number(price,'f',2)+"("+QString::number(p)+"%)");
            drawChart.timeShareVol->setText(GlobalVar::format_conversion(GlobalVar::mTimeShareChartList.at(n).vol));
            drawChart.timeShareTime->setText(GlobalVar::mTimeShareChartList.at(n).time.right(5));
            drawChart.timeSharePrice->adjustSize();
            drawChart.timeShareVol->adjustSize();
            drawChart.timeSharePrice->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.timeSharePrice->width()/(drawChart.timeShareChart->width()-2*WIDTHEDGE),100);
            drawChart.timeShareVol->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.timeShareVol->width()/(drawChart.timeShareChart->width()-2*WIDTHEDGE),250);
            drawChart.timeShareTime->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.timeShareTime->width()/(drawChart.timeShareChart->width()-2*WIDTHEDGE),220);
            drawChart.vLine.move(x,0);
            drawChart.hLine.move(0,y);
            drawChart.timeSharePrice->show();
            drawChart.timeShareVol->show();
            drawChart.timeShareTime->show();
            drawChart.vLine.show();
            drawChart.hLine.show();
        }
        else if (event->type()==QEvent::Leave)
        {
            drawChart.timeSharePrice->hide();
            drawChart.timeShareVol->hide();
            drawChart.timeShareTime->hide();
            drawChart.vLine.hide();
            drawChart.hLine.hide();
        }
        return true;
    }
    else if (obj==drawChart.hisTimeShareChartTitle)
    {
        if (event->type()==QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            drawChart.isMoveHisTimeShareChart=true;
            p=mouseEvent->pos();
        }
        else if (event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (drawChart.isMoveHisTimeShareChart)
                drawChart.hisTimeShareChart->move((mouseEvent->scenePosition()-p).rx(),(mouseEvent->scenePosition()-p).ry());
        }
        else if (event->type()==QEvent::MouseButtonRelease)
        {
            drawChart.isMoveHisTimeShareChart=false;
        }
        return true;
    }
    else if (obj==drawChart.hisTimeShareChartView)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter painter(drawChart.hisTimeShareChartView);
            drawChart.drawHisTimeShare(painter);
            painter.end();
        }
        else if (event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            int n=int(mouseEvent->pos().rx()*(GlobalVar::mHisTimeShareChartList.size())/(drawChart.hisTimeShareChartView->width()-2*WIDTHEDGE));

            if (n<0 or n>GlobalVar::mHisTimeShareChartList.size()-1 or
                mouseEvent->pos().ry()<TOPHEIGHTEDGE or
                mouseEvent->pos().ry()>drawChart.hisTimeShareChartView->height()-BOTTOMHEIGHTEDGE)
            {
                drawChart.hisTimeSharePrice->hide();
                drawChart.hisTimeShareVol->hide();
                drawChart.hisTimeShareTime->hide();
                drawChart.vHisLine.hide();
                drawChart.hHisLine.hide();
                return false;
            }
            drawChart.vHisLine.setStyleSheet("QLabel{border:2px dotted white;}");
            drawChart.vHisLine.resize(1,drawChart.hisTimeShareChartView->height()-BOTTOMHEIGHTEDGE);
            drawChart.hHisLine.setStyleSheet("QLabel{border:2px dotted white;}");
            drawChart.hHisLine.resize(drawChart.hisTimeShareChartView->width()-2*WIDTHEDGE,1);
            float price=GlobalVar::mHisTimeShareChartList.at(n).price;

            if (price>GlobalVar::hisPreClose)
                drawChart.hisTimeSharePrice->setStyleSheet("color:red;font:bold;font-size:18px");
            else
                drawChart.hisTimeSharePrice->setStyleSheet("color:white;font:bold;font-size:18px");
            float p=(price-GlobalVar::hisPreClose)*100/GlobalVar::hisPreClose;
            int x=(drawChart.hisTimeShareChartView->width()-2*WIDTHEDGE)*n/GlobalVar::mHisTimeShareChartList.size()+WIDTHEDGE;
            int y=(drawChart.hisTimeShareHighLowPoint[0]-price)*(drawChart.hisTimeShareChartView->height()*12/15-2*TOPHEIGHTEDGE)/(drawChart.hisTimeShareHighLowPoint[0]-drawChart.hisTimeShareHighLowPoint[1])+TOPHEIGHTEDGE;
//            qDebug()<<x<<y;
            drawChart.hisTimeSharePrice->setText(QString::number(price,'f',2)+"("+QString::number(p,'f',2)+"%)");
            drawChart.hisTimeShareVol->setText(GlobalVar::format_conversion(GlobalVar::mHisTimeShareChartList.at(n).vol));
            drawChart.hisTimeShareTime->setText(GlobalVar::mHisTimeShareChartList.at(n).time.right(5));
            drawChart.hisTimeSharePrice->adjustSize();
            drawChart.hisTimeShareVol->adjustSize();
            drawChart.hisTimeSharePrice->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.hisTimeSharePrice->width()/(drawChart.hisTimeShareChartView->width()-2*WIDTHEDGE),100);
            drawChart.hisTimeShareVol->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.hisTimeShareVol->width()/(drawChart.hisTimeShareChartView->width()-2*WIDTHEDGE),340);
            drawChart.hisTimeShareTime->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.hisTimeShareTime->width()/(drawChart.hisTimeShareChart->width()-2*WIDTHEDGE),310);
            drawChart.vHisLine.move(x,0);
            drawChart.hHisLine.move(0,y);
            drawChart.hisTimeSharePrice->show();
            drawChart.hisTimeShareVol->show();
            drawChart.hisTimeShareTime->show();
            drawChart.vHisLine.show();
            drawChart.hHisLine.show();
            drawChart.hisTimeShareChartView->update();
        }
        else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent *ev = static_cast<QWheelEvent *>(event);
            if (ev->angleDelta().y()<0)
            {
                hisTimeShareN+=1;
                if (hisTimeShareN>GlobalVar::mCandleChartList.count()-1)
                {
                    hisTimeShareN=GlobalVar::mCandleChartList.count()-1;
                    return true;
                }
            }
            else
            {
                hisTimeShareN-=1;
                if (hisTimeShareN<0)
                {
                    hisTimeShareN=0;
                    return true;
                }
            }
            QString date=GlobalVar::mCandleChartList.at(hisTimeShareN).time;
            mFundFlow.getTimeShareMin(GlobalVar::getStockSymbol(),date);
            drawChart.title->setText(GlobalVar::curName.left(GlobalVar::curName.indexOf("("))+" "+date+"分时图");
            drawChart.hisTimeShareChartView->update();
        }
        else if (event->type()==QEvent::Leave)
        {
            drawChart.hisTimeSharePrice->hide();
            drawChart.hisTimeShareVol->hide();
            drawChart.hisTimeShareTime->hide();
            drawChart.vHisLine.hide();
            drawChart.hHisLine.hide();
        }
        return true;
    }
    else if (obj==drawChart.candleChart)
    {
        if (event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            flashOldCandleInfo(mouseEvent);
        }
        else if (event->type()==QEvent::MouseButtonDblClick)
        {
            if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or
                GlobalVar::curCode.left(2)=="10")
            {
                QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
                return false;
            }
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (mouseEvent->pos().y()<30)
                return false;
            int m=(mouseEvent->pos().x()-KWIDTHEDGE)*GlobalVar::KRange/(drawChart.candleChart->width()-2*KWIDTHEDGE);
            if (GlobalVar::mCandleChartList.count()<GlobalVar::KRange)
                hisTimeShareN=m;
            else
                hisTimeShareN=GlobalVar::mCandleChartList.count()-GlobalVar::offsetLocal+m;
            if (hisTimeShareN>GlobalVar::mCandleChartList.count()-1 or hisTimeShareN<0)
                return true;
            QString date=GlobalVar::mCandleChartList.at(hisTimeShareN).time;
            mFundFlow.getTimeShareMin(GlobalVar::getStockSymbol(),date);
            drawChart.hisTimeShareChart->show();
            drawChart.title->setText(GlobalVar::curName.left(GlobalVar::curName.indexOf("("))+" "+date+"分时图");
            drawChart.hisTimeShareChartView->update();
        }
        else if (event->type()==QEvent::Paint)
        {
            QPainter painter(drawChart.candleChart);
            drawChart.drawCandleChart(painter);
            painter.end();
        }
        else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent *ev = static_cast<QWheelEvent *>(event);
            if (drawChart.hisTimeShareChart->isHidden())
                downUpLookStock(ev);
            else
            {
                if (ev->angleDelta().y()<0)
                {
                    hisTimeShareN+=1;
                    if (hisTimeShareN>GlobalVar::mCandleChartList.count()-1)
                    {
                        hisTimeShareN=GlobalVar::mCandleChartList.count()-1;
                        return true;
                    }
                }
                else
                {
                    hisTimeShareN-=1;
                    if (hisTimeShareN<0)
                    {
                        hisTimeShareN=0;
                        return true;
                    }
                }
                QString date=GlobalVar::mCandleChartList.at(hisTimeShareN).time;
                mFundFlow.getTimeShareMin(GlobalVar::getStockSymbol(),date);
                drawChart.title->setText(GlobalVar::curName.left(GlobalVar::curName.indexOf("("))+" "+date+"分时图");
                drawChart.hisTimeShareChartView->update();
            }
        }
        else if (event->type()==QEvent::Leave)
        {
            drawChart.colPrice->hide();
            drawChart.rowTime->hide();
            drawChart.vKLine->hide();
            drawChart.hKLine->hide();
            reFlashBuySellBaseInfo();
        }
        return true;
    }
    else if (obj==newsData->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
//        int maxStep=newsData->verticalScrollBar()->maximum();
        int tempStep=newsData->verticalScrollBar()->value();
//        qDebug()<<para<<tempStep;
        if (para<0/* and tempStep+6<maxStep*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep+136);
        else if (para>0/* and tempStep>6*/)
            newsData->verticalScrollBar()->setSliderPosition(tempStep-136);
        return true;
    }
    else if (obj==mTableStock.timeShareTickView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *ev = static_cast<QWheelEvent *>(event);
        int para=ev->angleDelta().y();
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
    else
    {
        for (int i=0;i<50;++i)
            if (obj==drawChart.annLabel[i] and event->type()==QEvent::MouseButtonDblClick)
            {
                QString date=drawChart.annLabel[i]->toolTip().left(11);
                int curItem=0;
                QStringList l;
                bool f=true;
                for(int j = 0; j < GlobalVar::annoucementList.count(); j++)
                {
                    if (f and date==GlobalVar::annoucementList.at(j)[2].left(11))
                    {
                        curItem=j;
                        f=false;
                    }
                    l<<GlobalVar::annoucementList.at(j)[2]+GlobalVar::annoucementList.at(j)[1]+
                            "\n"+GlobalVar::annoucementList.at(j)[0];
                }
                drawChart.model->setStringList(l);
                drawChart.annTitle->setModel(drawChart.model);
                QModelIndex qindex = drawChart.model->index(curItem,0);
                drawChart.annTitle->setCurrentIndex(qindex);
                drawChart.annoucementWindow->setWindowTitle(GlobalVar::curCode+" "+
                            GlobalVar::curName.left(GlobalVar::curName.indexOf("("))+" "+"消息");
                drawChart.annClicked(qindex);
                drawChart.annoucementWindow->show();
                return true;
            }
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
    if (key==Qt::Key_Escape and GlobalVar::isKState)
    {
        if (not drawChart.annoucementWindow->isHidden())
            drawChart.annoucementWindow->close();
        else if (not drawChart.hisTimeShareChart->isHidden())
            drawChart.hisTimeShareChart->close();
        else
        {
            GlobalVar::isKState=false;
            isLookMyStock=false;
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
            resetKParameter();
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
            mFundFlow.initAllNews();
            drawChart.candleChart->update();
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
            mFundFlow.initAllNews();
            drawChart.candleChart->update();
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
        toInterFace("f3");
    else if (key==Qt::Key_F10)
        toInterFace("f10");
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    downUpLookStock(event);
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
//        GlobalVar::isKState=true;
        resetKParameter();
        searchSmallWindow->hide();
        emit startThreadCandleChart(freq,adjustFlag,true);
        emit startThreadTimeShareChart();
        emit startThreadTimeShareTick();
        toInterFace("k");
    }
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
    dateEdit->setDate(GlobalVar::curRecentWorkDay(0).date());
    tradedetailBox->setItemText(0,GlobalVar::curRecentWorkDay(1).toString("yyyy-MM-dd"));

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
//    if (not ui->DLAllStockK->isEnabled() and curTime.time().toString("hh:mm")>"15:00")
//        ui->DLAllStockK->setEnabled(true);
    if (timeCount%2==1)
    {
        if (GlobalVar::WhichInterface==1 and GlobalVar::isZhMarketDay(curTime))
            if (GlobalVar::curCode.left(1)!="1" and GlobalVar::curCode.left(3)!="399")
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
            //A股交易时段实时刷新k线图
//            if (GlobalVar::isKState)
//                emit startThreadCandleChart(freq,adjustFlag,true);
            if (GlobalVar::curCode.left(2)=="1." or GlobalVar::curCode.left(3)=="399")
                emit startThreadTimeShareTick();
            emit startThreadTable();
            emit startThreadTimeShareChart();

        }
        //美股港股交易时段实时刷新所有股票，及分时图、分时成交
        else if ((GlobalVar::WhichInterface==5 and GlobalVar::isUSMarketDay(curTime)) or
                 (GlobalVar::WhichInterface==2 and GlobalVar::isHKMarketDay(curTime)))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
//            if (GlobalVar::isKState)
//                emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTable();
            emit startThreadTimeShareChart();
            emit startThreadTimeShareTick();
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
                requestsToCsv.downStockIndexPlateInfo();
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
            bl->setPalette(GlobalVar::pRed);
        else if (GlobalVar::mIndexList.at(n).pctChg.toFloat()<0)
            bl->setPalette(GlobalVar::pGreen);
        else
            bl->setPalette(GlobalVar::pBlack);
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
        bl->setPalette(GlobalVar::pRed);
    else if (GlobalVar::mIndexList.at(n).pctChg.toFloat()<0)
        bl->setPalette(GlobalVar::pGreen);
    else
        bl->setPalette(GlobalVar::pBlack);
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
    for (int i=0;i<7;++i)
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
            float p=GlobalVar::baseInfoData[i-1];
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
            baseInfoData[i]->setText(GlobalVar::format_conversion(GlobalVar::baseInfoData[i-1]));
    stockCode->setText(GlobalVar::curCode);
    stockName->setText(GlobalVar::curName);
    EPSLabel->setText(GlobalVar::EPSReportDate);
    PELabel->setText(GlobalVar::PEName);
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
    drawChart.rowTime->show();
    drawChart.colPrice->show();
    if (mouseEvent->pos().ry()>drawChart.candleChart->height()*12/15)
    {
        float vol=drawChart.candleHighLowPoint[2]-drawChart.candleHighLowPoint[2]*(mouseEvent->pos().ry()-drawChart.candleChart->height()*12/15-KBOTTOMHEIGHTEDGE)/(drawChart.candleChart->height()*3/15-2*KBOTTOMHEIGHTEDGE);
        drawChart.colPrice->setText(GlobalVar::format_conversion(int(vol)));
    }
    else
    {
        float price=drawChart.candleHighLowPoint[0]-(drawChart.candleHighLowPoint[0]-drawChart.candleHighLowPoint[1])*(mouseEvent->pos().ry()-KTOPHEIGHTEDGE)/(drawChart.candleChart->height()*12/15-2*KTOPHEIGHTEDGE);
        drawChart.colPrice->setText(QString::number(price,'f',2));
    }
    drawChart.colPrice->adjustSize();
    drawChart.rowTime->setText(GlobalVar::mCandleChartList.at(n).time);
    drawChart.colPrice->move(0,mouseEvent->pos().ry()-25);
    drawChart.rowTime->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-KWIDTHEDGE)*drawChart.rowTime->width()/(drawChart.candleChart->width()-2*KWIDTHEDGE),drawChart.candleChart->height()*12/15);
    drawChart.vKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    drawChart.vKLine->resize(1,drawChart.candleChart->height());
    drawChart.hKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    drawChart.hKLine->resize(drawChart.candleChart->width(),1);
    int posX=(2*m+1)*(drawChart.candleChart->width()-2*KWIDTHEDGE)/(2*GlobalVar::KRange);
//    int posY=;
    drawChart.vKLine->move(posX+KWIDTHEDGE,0);
    drawChart.hKLine->move(0,mouseEvent->pos().ry());
    drawChart.vKLine->show();
    drawChart.hKLine->show();
}
void MainWindow::toInterFace(QString which)
{
    if (which=="main")
    {
        GlobalVar::isKState=false;
        isLookMyStock=false;
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
        mFundFlow.initAllNews();
        drawChart.candleChart->update();
    }
    else if (which=="fund")
    {
        GlobalVar::WhichInterface=4;
        GlobalVar::isKState=false;
        isLookMyStock=false;
        rightBaseWindow->hide();
        drawChart.candleChart->hide();
        mTableStock.risingSpeedView->hide();
        mTableStock.myStockView->hide();
        rightFundWindow->show();
        mTableStock.stockTableView->show();
//        mTableStock.stockTableView->setFocus();
    }
    else if(which=="f3")
    {
        F10SmallWindow->setFixedSize(675,500);
        fTitle->setText(GlobalVar::curCode+" "+GlobalVar::curName.left(GlobalVar::curName.indexOf("(")));
        F10SmallWindow->show();
        f10View.dealWithHotRank();
    }
    else if(which=="f10")
    {
        F10SmallWindow->setFixedSize(1275,700);
        fTitle->setText(GlobalVar::curCode+" "+GlobalVar::curName.left(GlobalVar::curName.indexOf("(")));
        F10SmallWindow->show();
        f10View.dealWithMainIndex();
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
        ifCanClick=2;
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
void MainWindow::downUpLookStock(QWheelEvent *event)
{
    drawChart.hisTimeShareChart->close();
    drawChart.annoucementWindow->close();
    if (GlobalVar::isKState)
    {
        resetKParameter();
        int curRow=mTableStock.stockTableView->currentIndex().row();
        int counts=0;
        if (ifCanClick==2)
            counts=mFundFlow.model->rowCount()-1;
        else
        {
            if (isLookMyStock)
            {
                counts=GlobalVar::mMyStockList.count()-1;
                curRow=mTableStock.myStockView->currentIndex().row();
            }
            else
            {
                counts=GlobalVar::mTableList.count()-1;
            }

        }
        if (event->angleDelta().y()<0)
        {
            curRow++;
            if (curRow>counts)
                curRow=0;
        }
        else
        {
            curRow--;
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
            if (isLookMyStock)
            {
                GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
                mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curRow,0));
            }
            else
            {
                GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
            }
        }
        //            qDebug()<<GlobalVar::curCode;
        emit startThreadCandleChart(freq,adjustFlag,true);
        emit startThreadTimeShareChart();
        emit startThreadTimeShareTick();
        mFundFlow.initAllNews();
        drawChart.candleChart->update();
    }
}
