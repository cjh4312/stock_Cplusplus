#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Py_Initialize();
    pModule=PyImport_ImportModule("qmt");

    initGlobalVar();
    initThread();
    initInterface();
    initSettings();
    initSignals();
    tim = new QTimer(this);
    tim->setInterval(500);
    connect(tim,SIGNAL(timeout()),this,SLOT(tradingTimeRunThread()));
    tim->start();
}

MainWindow::~MainWindow()
{
    Py_Finalize();
    saveCode();
    requestsToCsv.progressBarWindow->close();
    QMTProcess->close();
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
    GlobalVar::settings=new QSettings(GlobalVar::currentPath+"/config.ini",QSettings::IniFormat,this);
    GlobalVar::curCode=GlobalVar::settings->value("curCode").toString();
    GlobalVar::isSayNews=GlobalVar::settings->value("sayNews").toBool();
    GlobalVar::offsetEnd=GlobalVar::settings->value("offsetEnd").toInt();
    downloadDate=GlobalVar::settings->value("curTime").toString();
    account=GlobalVar::settings->value("account").toString();
//    QString path=GlobalVar::currentPath+"/python/";
//    Py_SetPythonHome((wchar_t *)(reinterpret_cast<const wchar_t *>(path.utf16())));
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
    connect(threadTable,&ThreadTable::getTableDataFinished,this,[=](){
        mTableStock.setTableView();
        baseInfoData[7]->setText(QString::number(GlobalVar::upNums[0]));
        baseInfoData[16]->setText(QString::number(GlobalVar::downNums[0]));
        baseInfoData[8]->setText(QString::number(GlobalVar::upNums[1])+"/"+QString::number(GlobalVar::upNums[2]));
        baseInfoData[17]->setText(QString::number(GlobalVar::downNums[1])+"/"+QString::number(GlobalVar::downNums[2]));
    });
    connect(this,&MainWindow::startThreadTable,threadTable,&ThreadTable::getTableData);
    thread[0]->start();
    emit startThreadTable();

    threadIndex=new ThreadIndex;
    threadIndex->moveToThread(thread[1]);
    connect(threadIndex,SIGNAL(getIndexFinished()),this,SLOT(reFlashIndex()));
    connect(threadIndex,&ThreadIndex::getBlockFinished,this,[=](){
        int row=mTableStock.blockView->currentIndex().row();
        if (row==-1)
            row=0;
        mTableStock.m_fundFlowModel->setModelData(GlobalVar::mFundFlowList,false);
        mTableStock.blockView->setCurrentIndex(mTableStock.m_fundFlowModel->index(row,0));
    });
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
    emit startThreadTimeShareTick(false);

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
//        QWidget *pActiveWindow = QApplication::activeWindow();
//        MainWindow *pMainWindow = dynamic_cast<MainWindow*>(pActiveWindow);
//        if(pMainWindow && pMainWindow == this)
            drawChart.timeShareChart->update();

    });
    connect(this,&MainWindow::startThreadTimeShareChart,threadTimeShareChart,&ThreadTimeShareChart::getAllTimeShareChart);
    thread[4]->start();
    emit startThreadTimeShareChart(false);

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
    setWindowIcon(QIcon(":/new/png/png/logo.ico"));
    // setWindowState(Qt::WindowMaximized);
    if (not GlobalVar::isSayNews)
        ui->newsReport->setChecked(true);
    // if (GlobalVar::settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
    //     ui->setVacation->setEnabled(false);
    QActionGroup *market = new QActionGroup(this);
    market->addAction(ui->HKMarket);
    market->addAction(ui->USMarket);
    market->addAction(ui->ZHMarket);
    ui->ZHMarket->setChecked(true);

    ui->horizontalLayout->setSpacing(0);
    ui->horizontalLayout->addWidget(mTableStock.stockTableView);
    ui->horizontalLayout->addWidget(drawChart.candleChart);
    drawChart.candleChart->hide();

    middleWindow=new QWidget(this);
    QVBoxLayout *middleLayout =new QVBoxLayout;
    QHBoxLayout *middleDLayout=new QHBoxLayout;
    middleWindow->setLayout(middleLayout);
    middleLayout->setSpacing(0);
    middleLayout->setContentsMargins(0,0,0,0);
    middleDLayout->setSpacing(0);
    middleDLayout->setContentsMargins(0,0,0,0);
    // mTableStock.blockView->setMaximumHeight(454);
    mTableStock.risingSpeedView->setMaximumWidth(285);
    mTableStock.risingSpeedView->setMinimumHeight(496);
    mTableStock.myStockView->setMinimumHeight(496);
    mTableStock.blockView->setMinimumWidth(785);
    middleLayout->addWidget(mTableStock.blockView);
    middleLayout->addLayout(middleDLayout);
    middleDLayout->addWidget(mTableStock.risingSpeedView);
    // middleDLayout->addWidget(mTableStock.blockView);
    middleDLayout->addWidget(mTableStock.myStockView);

    rightBaseWindow=new QWidget(this);
    rightBaseWindow->setMaximumWidth(450);
    QVBoxLayout *rightLayout =new QVBoxLayout(rightBaseWindow);
    rightBaseWindow->setLayout(rightLayout);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0,0,0,0);
    //2
    rightFundWindow=new QWidget(this);
    rightFundWindow->setMaximumWidth(250);
    QVBoxLayout *rightFundLayout=new QVBoxLayout(rightFundWindow);
    rightFundWindow->setLayout(rightFundLayout);
//    rightFundLayout->setSpacing(0);
    rightFundLayout->setContentsMargins(20,30,20,2);

    ui->horizontalLayout->addWidget(middleWindow);
    ui->horizontalLayout->addWidget(rightBaseWindow);
    ui->horizontalLayout->addWidget(rightFundWindow);
    rightFundWindow->hide();

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
        periodAdjust[i]=new QRadioButton(periodAdjustName[i],this);
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
        fundFlow[i]=new QPushButton(fundFlowName[i],this);
        rightFundLayout->addWidget(fundFlow[i]);
    }
    rightFundLayout->addWidget(openFundBox);
    QTextBrowser *instructionText=new QTextBrowser(this);
    instructionText->setStyleSheet("background-color:#F3F3F3;");
//    instructionText->setMaximumHeight(300);
    rightFundLayout->addWidget(instructionText);
    instructionText->append(GlobalVar::settings->value("Instruction").toString());

    circle=new QLabel(this);
    ui->statusBar->addWidget(circle);
    circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
    for(int i=1;i<=8;++i)
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
    this->setAttribute(Qt::WA_DeleteOnClose);
    if (GlobalVar::isSayNews)
    {
        QIcon icon(":/new/png/png/report.png");
        ui->newsReport->setIcon(icon);
        ui->newsReport->setText("关闭语音播报");
    }
    else
    {
        QIcon icon(":/new/png/png/no_report.png");
        ui->newsReport->setIcon(icon);
        ui->newsReport->setText("打开语音播报");
    }
    newsData->setOpenExternalLinks(true);
    drawChart.hisTimeShareChart->setParent(this);

    mTableStock.stockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.risingSpeedView->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableStock.myStockView->setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    mTableStock.stockTableView->setFocusPolicy(Qt::NoFocus);
    mTableStock.blockView->setFocusPolicy(Qt::NoFocus);
    mTableStock.risingSpeedView->setFocusPolicy(Qt::NoFocus);
    mTableStock.myStockView->setFocusPolicy(Qt::ClickFocus);
    mTableStock.myStockView->horizontalHeader()->setHighlightSections(false);

    QScreen* screen = QGuiApplication::primaryScreen();
    searchSmallWindow=new QWidget(this);
    searchSmallWindow->setWindowFlag(Qt::Popup);
    int w=screen->size().width();
    int h=screen->size().height();
    searchSmallWindow->setGeometry(w-450, h-435, 300, 350);
    QVBoxLayout *search =new QVBoxLayout(searchSmallWindow);
    search->setContentsMargins(2, 2, 2, 2);
    searchSmallWindow->setLayout(search);
    search->addWidget(searchStock.searchCodeLine);
    search->addWidget(searchStock.matchCodeText);

    F10SmallWindow=new QWidget(this);
    QWidget *F10Title=new QWidget(this);
    F10SmallWindow->setWindowFlag(Qt::Popup);
    F10SmallWindow->move(200,100);
    F10SmallWindow->hide();
    QVBoxLayout *f10MainLayout=new QVBoxLayout(F10SmallWindow);
    F10SmallWindow->setLayout(f10MainLayout);
    QHBoxLayout *f10ContentLayout =new QHBoxLayout();
    QHBoxLayout *titleLayout=new QHBoxLayout(F10Title);

    QPushButton *close=new QPushButton(F10SmallWindow);
    connect(close,&QPushButton::clicked,this,[=](){F10SmallWindow->close();});
    F10Title->setLayout(titleLayout);
    F10Title->setMaximumHeight(TITLEHEIGHT);
//    f10Title->setStyleSheet("background-color:rgb(153, 204, 255);");
    fTitle->setStyleSheet("QLabel{font:bold 18px;font-family:微软雅黑}");

    f10MainLayout->addWidget(F10Title);
    f10MainLayout->addLayout(f10ContentLayout);
    f10MainLayout->setContentsMargins(0,0,0,0);

    QIcon myicon;
    myicon.addFile(tr(":/new/png/png/close.png"));
    close->setIcon(myicon);
    close->setIconSize(QSize(20,20));
    close->setMaximumSize(QSize(30,30));
    titleLayout->addWidget(fTitle);
    titleLayout->addWidget(close);

    QWidget *navigation=new QWidget(F10SmallWindow);
    f10ContentLayout->addWidget(navigation);
    f10ContentLayout->addWidget(f10View.stockInfoView);
    QVBoxLayout *navi=new QVBoxLayout(navigation);
    navigation->setLayout(navi);
    QStringList F10InfoName={"热度概念排名","主要指标","经营分析","资产负债表","利润表","现金流量表"};
    for (int i=0;i<6;++i)
    {
        F10Info[i]=new QPushButton(F10InfoName[i],F10SmallWindow);
        navi->addWidget(F10Info[i]);
    }
}
void MainWindow::initBaseInfoLayout(QGridLayout *baseInfoLayout)
{
    stockCode = new QLabel(this);
    stockName = new QLabel(this);
    stockCode->setStyleSheet("QLabel{font:bold;color:blue}");
    stockName->setStyleSheet("QLabel{font:bold 26px;font-family:微软雅黑;color:red}");
    // baseInfoLayout->setVerticalSpacing(1);
    baseInfoLayout->addWidget(stockCode, 0, 0, 2, 1);
    baseInfoLayout->addWidget(stockName, 0, 1, 2, 3);

    QString lName[]={"现价", "涨幅","换手", "成交额", "总股本", "总市值", "市净率", "上涨总数","上涨5/8",
                       "今开", "最高", "最低", "成交量(手)", "流通股", "", "", "下跌总数","下跌5/8"};

    for (int i=0;i<18;++i)
    {
        QLabel *name=new QLabel(lName[i],this);
        baseInfoData[i]=new QLabel(this);
        name->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
        if ((i >= 2 and i <= 6) or (i >= 12 and i <= 15))
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:blue}");
        else if (i == 7 or i==8)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:red}");
//            name->setMaximumWidth(90);
        }
        else if (i == 16 or i==17)
        {
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px;color:green}");
//            name->setMaximumWidth(90);
        }
        else
            baseInfoData[i]->setStyleSheet("QLabel{font:bold 14px}");
        if (i < 9)
        {
            baseInfoLayout->addWidget(name,i+2,0);
            baseInfoLayout->addWidget(baseInfoData[i],i+2,1);
        }
        else if (i<18)
        {
            baseInfoLayout->addWidget(name,i-7,2);
            baseInfoLayout->addWidget(baseInfoData[i],i-7,3);
        }
    }
    EPSLabel=new QLabel(this);
    EPSLabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
    PELabel=new QLabel(this);
    PELabel->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(47,79,79)}");
    baseInfoLayout->addWidget(EPSLabel,7,2);
    baseInfoLayout->addWidget(PELabel,8,2);
}
void MainWindow::initBuySellLayout(QGridLayout *BuySellLayout)
{
    QString n[5]={"一","二", "三", "四", "五"};
    QLabel *buySellName[10];
    for (int i=0;i<5;++i)
    {
        buySellName[i]=new QLabel("卖"+n[4-i],this);
        buySellPrice[i]=new QLabel(this);
        buySellPrice[i]->setContextMenuPolicy(Qt::CustomContextMenu);
        buySellNum[i]=new QLabel(this);
        buySellNum[i]->setAlignment(Qt::AlignRight);
        buySellName[i]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(47,79,79)}");
        buySellPrice[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i]->setStyleSheet("QLabel{font:bold 16px;color:rgb(255,165,0);font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i],i,0);
        BuySellLayout->addWidget(buySellPrice[i],i,1);
        BuySellLayout->addWidget(buySellNum[i],i,2);

        buySellName[i+5]=new QLabel("买"+n[i],this);
        buySellPrice[5+i]=new QLabel(this);
        buySellPrice[5+i]->setContextMenuPolicy(Qt::CustomContextMenu);
        buySellNum[5+i]=new QLabel(this);
        buySellNum[5+i]->setAlignment(Qt::AlignRight);
        buySellName[i+5]->setStyleSheet("QLabel{font:bold 16px;font-family:微软雅黑;color:rgb(47,79,79)}");
        buySellPrice[i+5]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
        buySellNum[i+5]->setStyleSheet("QLabel{font:bold 16px;color:rgb(255,165,0);font-family:微软雅黑}");
        BuySellLayout->addWidget(buySellName[i+5],i+6,0);
        BuySellLayout->addWidget(buySellPrice[5+i],i+6,1);
        BuySellLayout->addWidget(buySellNum[5+i],i+6,2);
    }
    buySellName[0]->setMinimumWidth(40);
    buySellPrice[0]->setMinimumWidth(45);
    buySellNum[0]->setMinimumWidth(40);
    QFrame *line=new QFrame(this);
    line->setStyleSheet("QFrame{/*background:yellow;*/min-height:2px;border-top:2px dotted gray}");
//    line->setFrameShape(QFrame::HLine);
//    line->setFrameShadow(QFrame::Sunken);
    BuySellLayout->addWidget(line,5,0,1,3);

    mTableStock.timeShareTickView->setMinimumWidth(300);
    BuySellLayout->addWidget(mTableStock.timeShareTickView,0,3,11,3);
}
void MainWindow::initSignals()
{
    connect(mTableStock.stockTableView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int row=index.row();
        if (GlobalVar::WhichInterface==4)
        {
            if(ifCanClick==1)
            {
                GlobalVar::curCode=GlobalVar::mTableList.at(row).code;
                GlobalVar::curName=GlobalVar::mTableList.at(row).name;
            }
            else if(ifCanClick==2)
            {
                GlobalVar::curCode=mFundFlow.model->item(row,0)->text();
                GlobalVar::curName=mFundFlow.model->item(row,1)->text();
            }
            else if(ifCanClick==0)
            {
                mFundFlow.isClick=true;
                mFundFlow.getFundFlowChartData(mFundFlow.model->item(row,13)->text());
                mFundFlow.fundFlowChart->setWindowTitle(mFundFlow.model->item(row,0)->text()+" 资金流图表");
                mFundFlow.fundFlowChart->show();
                mFundFlow.fundFlowChart->update();
                mFundFlow.fundFlowChart->move(POSXCHART,POSYCHART);
                mFundFlow.fundFlowChart->move(POSXCHART+1,POSYCHART);
            }
        }
        else
        {
            GlobalVar::curCode=GlobalVar::mTableList.at(row).code;
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
        }
    });
    connect(mTableStock.risingSpeedView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mRisingSpeedList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mRisingSpeedList.at(curRow).name;
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
    });
    connect(mTableStock.myStockView, &QTableView::clicked, this, [this](const QModelIndex &index){
        int curRow=index.row();
        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
    });
    // connect(mTableStock.blockView,&QTableView::clicked,this, [this](const QModelIndex &index){
    //     int row=index.row();
    //     mFundFlow.isClick=true;
    //     mFundFlow.getFundFlowChartData(GlobalVar::mFundFlowList.at(row)[13]);
    //     mFundFlow.fundFlowChart->setWindowTitle(GlobalVar::mFundFlowList.at(row)[0]+" 资金流图表");
    //     mFundFlow.fundFlowChart->show();
    //     mFundFlow.fundFlowChart->update();
    //     mFundFlow.fundFlowChart->move(859,150);
    //     mFundFlow.fundFlowChart->move(860,150);
    // });
    connect(ui->ZHMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->HKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->USzMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(ui->UKMarket,SIGNAL(triggered()),this,SLOT(setMarket()));
    connect(mTableStock.stockTableView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        if (GlobalVar::WhichInterface==4)
        {
            if (ifCanClick==0)
            {
                mFundFlow.getBoardStock(mFundFlow.FundFlowList.at(index.row())[0]);
                mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
                mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
                for (int i=0;i<16;++i)
                    mTableStock.stockTableView->setColumnWidth(i,90);
                ifCanClick=1;
            }
            else if(ifCanClick==1 or ifCanClick==2)
            {
                GlobalVar::isKState=true;
                emit startThreadCandleChart(freq,adjustFlag,true);
                emit startThreadTimeShareTick(false);
                emit startThreadTimeShareChart(false);
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
    connect(mTableStock.blockView, &QTableView::doubleClicked, this, [this](const QModelIndex &index){
        mTableStock.isFlashTable=false;
        GlobalVar::isBoard=true;
        GlobalVar::curBoard=GlobalVar::mFundFlowList.at(index.row())[13];
        searchStock.getBoardData();
        if (not GlobalVar::mTableList.isEmpty())
            mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
        mTableStock.isFlashTable=true;

    });
    connect(mTableStock.myStockView, &QTableView::doubleClicked, this, [this](const QModelIndex &/*index*/){
        GlobalVar::isKState=true;
        isTraversalMyStock=true;
//        int curRow=index.row();
//        GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
//        GlobalVar::curName=GlobalVar::mMyStockList.at(curRow).name;
        emit startThreadCandleChart(freq,adjustFlag,true);
        toInterFace("k");
    });
    // 信号发出，进行排序
    connect(mTableStock.stockTableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, [this](int logicalIndex/*, Qt::SortOrder order*/) {
        //        bool is_asc;
        if (GlobalVar::WhichInterface!=4 or(GlobalVar::WhichInterface==4 and ifCanClick==1))
        {
            if (GlobalVar::curSortNum!=logicalIndex)
            {
                GlobalVar::is_asc = false;
                GlobalVar::curSortNum=logicalIndex;
            }
            else
                GlobalVar::is_asc = not preSort;
            GlobalVar::sortByColumn(&GlobalVar::mTableList,logicalIndex,GlobalVar::is_asc);
            preSort=GlobalVar::is_asc;
            mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
            // mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
            mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
            mTableStock.stockTableView->scrollTo(mTableStock.m_tableModel->index(0,0));
        }
//        else if (GlobalVar::WhichInterface==4 and ifCanClick==2)
//        {
//            mFundFlow.model->sort(logicalIndex,Qt::DescendingOrder);
//            mTableStock.stockTableView->setModel(mFundFlow.model);
//            mTableStock.stockTableView->setCurrentIndex(mFundFlow.model->index(0,0));
//        }
    });
    connect(mTableStock.stockTableView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        if (GlobalVar::WhichInterface==1 or (GlobalVar::WhichInterface==4 and (ifCanClick==2 or ifCanClick==1)))
            addRightMenu(1);
    });
    connect(mTableStock.risingSpeedView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        addRightMenu(2);
    });
    connect(mTableStock.myStockView,&QTableView::customContextMenuRequested,this,[=](QPoint){
        QMenu *menu=new QMenu(this);
        QAction *act=new QAction("删除自选",menu);
        menu->setAttribute(Qt::WA_DeleteOnClose);
        QIcon icon(":/new/png/png/del.jpg");
        act->setIcon(icon);
        menu->addAction(act);
        menu->popup(QCursor::pos());
        connect(act,&QAction::triggered,this,&MainWindow::delMyStock);
    });
    for (int i = 2; i<=16; i=i+2)
    {
        QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(i));
        connect(bb,&QPushButton::clicked,this,[=](){
            if (not drawChart.hisTimeShareChart->isHidden())
                drawChart.hisTimeShareChart->close();
            GlobalVar::isKState=true;
            QString IndexCode[]={"1.000001","399001","399006","100.HSI","100.N225","100.KS11","1.000688","100.TWII","100.SENSEX","100.DJIA",
                                "100.SPX","100.NDX","100.SX5E","100.GDAXI","100.RTS","100.FTSE","100.FCHI","100.AS51","104.CN00Y","103.YM00Y",
                                "133.USDCNH","122.XAU"};
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
                    n=19;
                else
                    n=18;
            }
            else if (i==16)
            {
                if (changeInTurn)
                    n=21;
                else
                    n=20;}
//            qDebug()<<i<<n;
            GlobalVar::curCode=IndexCode[n];
            GlobalVar::curName=bb->text();
            resetKParameter();
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
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
        if (GlobalVar::isSayNews)
        {
            QIcon icon(":/new/png/png/report.png");
            ui->newsReport->setIcon(icon);
            ui->newsReport->setText("关闭语音播报");
        }
        else
        {
            QIcon icon(":/new/png/png/no_report.png");
            ui->newsReport->setIcon(icon);
            ui->newsReport->setText("打开语音播报");
        }
    });
    connect(ui->fundFlow,&QAction::triggered,this,&MainWindow::dealWithFundFlow);
    connect(&searchStock,SIGNAL(showSearch()),this,SLOT(showSearchResult()));
    connect(ui->DLAllStockK,&QAction::triggered,this,[=](){
        requestsToCsv.downloadAllStockK();
    });
    connect(ui->DLStockInfo,&QAction::triggered,this,[=](){
        requestsToCsv.downStockIndexPlateInfo();
    });
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
    connect(f10View.stockInfoView,&QTableView::doubleClicked,this,[=](const QModelIndex &index){
        GlobalVar::curBoard=f10View.model->item(index.row(),3)->text();
        GlobalVar::isBoard=true;
        searchStock.getBoardData();
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
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
    connect(ui->formula,&QAction::triggered,this,[=](){
        QDialog *formulaDes=new QDialog(this);
        formulaDes->setWindowTitle("编写公式说明");
        formulaDes->setGeometry(450, 200, 1000, 700);
        formulaDes->setAttribute(Qt::WA_DeleteOnClose);
        QTextBrowser *des=new QTextBrowser(formulaDes);
        des->setMinimumHeight(380);
        des->setStyleSheet("QTextBrowser{font:bold 22px;font:bold}");
        QVBoxLayout *mainLayout =new QVBoxLayout(formulaDes);
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
        d->insertHtml("<img src=:/new/png/png/des.jpg/>");
        formulaDes->show();
        file.close();
    });
    connect(ui->setVacation,&QAction::triggered,this,[=](){
        if (GlobalVar::getVacation())
            QMessageBox::information(this,"提示", "设置成功。一年只需设置一次！", QMessageBox::Ok);
        else
            QMessageBox::information(this,"提示", "设置失败,请重试", QMessageBox::Ok);
        // if (GlobalVar::settings->value("isSetVacation").toString()==QDateTime::currentDateTime().toString("yyyy"))
        //     ui->setVacation->setEnabled(false);
    });
    for (int i=0;i<6;++i)
    {
        connect(mFundFlow.checkBox[i],&QCheckBox::clicked,this,[=](){
            if (mFundFlow.checkBox[i]->isChecked())
                mFundFlow.isShow[i]=true;
            else
                mFundFlow.isShow[i]=false;
            mFundFlow.tableChart->update();
        });
        connect(periodAdjust[i],&QRadioButton::clicked,this,[=](){
            if (i==0)
                freq="101";
            else if (i==1)
                freq="102";
            else if (i==2)
                freq="103";
            else if (i==3)
                adjustFlag="0";
            else if (i==4)
                adjustFlag="1";
            else if (i==5)
                adjustFlag="2";
            if (GlobalVar::isKState)
            {
                preCode="";
                //        resetKParameter();
                emit startThreadCandleChart(freq,adjustFlag,true);
            }
        });
    }
    for (int i=0;i<10;++i)
    {
        connect(fundFlow[i],&QPushButton::clicked,this,&MainWindow::toFundFlow);
        connect(buySellPrice[i],&QLabel::customContextMenuRequested,this,&MainWindow::fastTrade);
    }
    connect(ui->login,&QAction::triggered,this,[=](){
        if (QMTProcess->state()==QProcess::NotRunning)
        {
            QString str = "D:\\Program Files\\Finance\\国金证券QMT交易端\\bin.x64\\XtMiniQmt.exe";
            QString tagDir = "\"" + str + "\"";
            QMTProcess->startDetached(tagDir);
        }
        else
            QMessageBox::information(this,"提示", "交易已经启动", QMessageBox::Ok);
    });
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
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mTableStock.stockTableView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=mTableStock.stockTableView->verticalScrollBar()->value();
        int curIndex=mTableStock.stockTableView->currentIndex().row();
        int row=mTableStock.stockTableView->height()/22;
        if (para<0)
        {
            mTableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>GlobalVar::mTableList.count()-row)
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(0,0));
            else
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curIndex+row,0));
        }
        else
        {
            mTableStock.stockTableView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curIndex-row,0));
            else
                mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(GlobalVar::mTableList.count()-1,0));

        }
//        qDebug()<<mTableStock.stockTableView->height();
        return true;
    }
    else if (obj==mTableStock.blockView->verticalScrollBar() and event->type() == QEvent::Wheel)
    {
        QWheelEvent *event1 = static_cast<QWheelEvent *>(event);
        int para=event1->angleDelta().y();
        int tempStep=mTableStock.blockView->verticalScrollBar()->value();
        int curIndex=mTableStock.blockView->currentIndex().row();
        int row=mTableStock.blockView->height()/22;
        if (para<0)
        {
            mTableStock.blockView->verticalScrollBar()->setSliderPosition(tempStep+row);
            if (curIndex>GlobalVar::mFundFlowList.count()-row)
                mTableStock.blockView->setCurrentIndex(mTableStock.m_fundFlowModel->index(0,0));
            else
                mTableStock.blockView->setCurrentIndex(mTableStock.m_fundFlowModel->index(curIndex+row,0));
        }
        else
        {
            mTableStock.blockView->verticalScrollBar()->setSliderPosition(tempStep-row);
            if (curIndex>=row)
                mTableStock.blockView->setCurrentIndex(mTableStock.m_fundFlowModel->index(curIndex-row,0));
            else
                mTableStock.blockView->setCurrentIndex(mTableStock.m_fundFlowModel->index(GlobalVar::mFundFlowList.count()-1,0));
        }
        //        qDebug()<<mTableStock.stockTableView->height();
        return true;
    }
    else if (obj==mTableStock.myStockView and event->type()==QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((keyEvent->key() >= 48 and keyEvent->key() <= 57) or (keyEvent->key() >= 65 and keyEvent->key() <= 90))
        {
            searchSmallWindow->show();
            searchStock.searchCodeLine->setText(keyEvent->text());
            searchStock.searchCodeLine->setFocus();
            searchStock.matchCodeText->moveCursor(QTextCursor::Start);
        }
    }
    else if (obj==drawChart.timeShareChart)
    {
        if (event->type() == QEvent::Paint and not timeShareRunning)
        {
            timeShareRunning=true;
            QPainter *painter=new QPainter(drawChart.timeShareChart);
            drawChart.drawTimeShareChart(painter);
            delete painter;
            timeShareRunning=false;
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
            else if (p<0)
                drawChart.timeSharePrice->setStyleSheet("color:rgb(0,255,0);font:bold;font-size:18px");
            else
                drawChart.timeSharePrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
            float price=(1+p/100)*GlobalVar::preClose;
            int x=(drawChart.timeShareChart->width()-2*WIDTHEDGE)*n/GlobalVar::trendsTotal+WIDTHEDGE;
            int y=(GlobalVar::timeShareHighLowPoint[0]-p)*(drawChart.timeShareChart->height()*12/15-2*TOPHEIGHTEDGE)/(GlobalVar::timeShareHighLowPoint[0]-GlobalVar::timeShareHighLowPoint[1])+TOPHEIGHTEDGE;
            //        qDebug()<<x<<y;
            drawChart.timeSharePrice->setText(QString::number(price,'f',2)+"("+QString::number(p)+"%)");
            drawChart.timeShareVol->setText(GlobalVar::format_conversion(GlobalVar::mTimeShareChartList.at(n).vol));
            drawChart.timeShareTime->setText(GlobalVar::mTimeShareChartList.at(n).time.right(5));
            drawChart.timeSharePrice->adjustSize();
            drawChart.timeShareVol->adjustSize();
            drawChart.timeSharePrice->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-WIDTHEDGE)*drawChart.timeSharePrice->width()/(drawChart.timeShareChart->width()-2*WIDTHEDGE),95);
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
    else if (obj==mFundFlow.tableChart)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter *painter=new QPainter(mFundFlow.tableChart);
            mFundFlow.drawIntervalHighLowChart(painter);
            delete painter;
        }
        return true;
    }
    else if (obj==mFundFlow.fundFlowChart)
    {
        if (event->type() == QEvent::Paint)
        {
            QPainter *painter=new QPainter(mFundFlow.fundFlowChart);
            painter->setRenderHint(QPainter::Antialiasing);
            mFundFlow.drawFundFlowChart(painter);
            delete painter;
        }
        else if(event->type()==QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            int maxNums=mFundFlow.fundFlowKList.count();

            int maxHNums=mFundFlow.fundFlowHKList.count();
            int aveHW=mFundFlow.fundFlowChart->width()/2/maxHNums;
            if (mouseEvent->pos().rx()>10 and
                mouseEvent->pos().rx()<mFundFlow.fundFlowChart->width()/2-10 and
                mouseEvent->pos().ry()>0 and
                mouseEvent->pos().ry()<(mFundFlow.fundFlowChart->height()-150)/2)
            {
                int n=0;
                if (maxNums!=0)
                {
                    int aveW=mFundFlow.fundFlowChart->width()/2/maxNums;
                    n=(mouseEvent->pos().rx()-10)/aveW;
                }
                if (maxNums-1>=n)
                {
                    mFundFlow.vKLine->show();
                    mFundFlow.vKLine->move(mouseEvent->pos().rx(),0);
                    mFundFlow.time->setText(mFundFlow.fundFlowKList.at(n)[0]);
                    if (mFundFlow.fundFlowKList.at(n)[1].toFloat()<0)
                        mFundFlow.textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        mFundFlow.textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");

                    mFundFlow.textFund[0]->setText(GlobalVar::format_conversion(mFundFlow.fundFlowKList.at(n)[1].toFloat()));
                    for (int i=1;i<5;++i)
                    {
                        if (mFundFlow.fundFlowKList.at(n)[6-i].toFloat()<0)
                            mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                        else
                            mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                        mFundFlow.textFund[i]->setText(GlobalVar::format_conversion(mFundFlow.fundFlowKList.at(n)[6-i].toFloat()));
                    }
                }
                else
                    mFundFlow.vKLine->hide();
            }
            else if (mouseEvent->pos().rx()>5 and
                       mouseEvent->pos().rx()<mFundFlow.fundFlowChart->width()/2-15 and
                       mouseEvent->pos().ry()>(mFundFlow.fundFlowChart->height()+70)/2 and
                       mouseEvent->pos().ry()<mFundFlow.fundFlowChart->height()-40)
            {
                int n=(mouseEvent->pos().rx()-5)/aveHW;
                if (n>=maxHNums)
                    return false;
                mFundFlow.vKLine->show();
                mFundFlow.vKLine->move(mouseEvent->pos().rx(),435);
                mFundFlow.time->setText(mFundFlow.fundFlowHKList.at(n)[0]);
                if (mFundFlow.fundFlowHKList.at(n)[1].toFloat()<0)
                    mFundFlow.textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                else
                    mFundFlow.textFund[0]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                mFundFlow.textFund[0]->setText(GlobalVar::format_conversion(mFundFlow.fundFlowHKList.at(n)[1].toFloat()));
                for (int i=1;i<5;++i)
                {
                    if (mFundFlow.fundFlowHKList.at(n)[6-i].toFloat()<0)
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                    mFundFlow.textFund[i]->setText(GlobalVar::format_conversion(mFundFlow.fundFlowHKList.at(n)[6-i].toFloat()));
                }
            }
            else if (mouseEvent->pos().rx()>mFundFlow.fundFlowChart->width()/2 and
                       mouseEvent->pos().rx()<mFundFlow.fundFlowChart->width()-15 and
                       mouseEvent->pos().ry()>(mFundFlow.fundFlowChart->height()+70)/2 and
                       mouseEvent->pos().ry()<mFundFlow.fundFlowChart->height()-205)
            {
                mFundFlow.time->setText("一周5日资金流入");
                for (int i=0;i<5;++i)
                {
                    if (mFundFlow.fiveTotal[i]<0)
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");

                    mFundFlow.textFund[i]->setText(GlobalVar::format_conversion(mFundFlow.fiveTotal[i]));
                }
            }
            else if (mouseEvent->pos().rx()>mFundFlow.fundFlowChart->width()/2 and
                       mouseEvent->pos().rx()<mFundFlow.fundFlowChart->width()-15 and
                       mouseEvent->pos().ry()>mFundFlow.fundFlowChart->height()-205 and
                       mouseEvent->pos().ry()<mFundFlow.fundFlowChart->height()-40)
            {
                mFundFlow.time->setText("一月20日资金流入");
                for (int i=0;i<5;++i)
                {
                    if (mFundFlow.twentyTotal[i]<0)
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(0,191,0)}");
                    else
                        mFundFlow.textFund[i]->setStyleSheet("QLabel{font:bold 16px;font:bold;font-family:微软雅黑;color:rgb(255,0,0)}");
                    mFundFlow.textFund[i]->setText(GlobalVar::format_conversion(mFundFlow.twentyTotal[i]));
                }
            }
            else if ((mouseEvent->pos().rx()-760)*(mouseEvent->pos().rx()-760)+
                           (mouseEvent->pos().ry()-175)*(mouseEvent->pos().ry()-175)<=125*125)
            {
                float degree=90-atan2(175-mouseEvent->pos().ry(),760-mouseEvent->pos().rx())*180/PI;
                if (degree<0)
                    degree+=360;
                if (degree<mFundFlow.degree[0])
                    mFundFlow.whichPiece=1;
                else if (degree<mFundFlow.degree[1])
                    mFundFlow.whichPiece=2;
                else if (degree<mFundFlow.degree[2])
                    mFundFlow.whichPiece=3;
                else if (degree<mFundFlow.degree[3])
                    mFundFlow.whichPiece=4;
                else if (degree<mFundFlow.degree[4])
                    mFundFlow.whichPiece=5;
                else if (degree<mFundFlow.degree[5])
                    mFundFlow.whichPiece=6;
                else if (degree<mFundFlow.degree[6])
                    mFundFlow.whichPiece=7;
                else
                    mFundFlow.whichPiece=8;
                mFundFlow.fundFlowChart->update();
            }
            else
            {
                mFundFlow.whichPiece=-1;
                mFundFlow.vKLine->hide();
                mFundFlow.fundFlowChart->update();
            }
            mFundFlow.fundFlowChart->move(POSXCHART,POSYCHART);
            mFundFlow.fundFlowChart->move(POSXCHART+1,POSYCHART);
        }
        else if (event->type()==QEvent::Leave)
        {
            mFundFlow.vKLine->hide();
            mFundFlow.fundFlowChart->move(POSXCHART,POSYCHART);
            mFundFlow.fundFlowChart->move(POSXCHART+1,POSYCHART);
        }
        else if (event->type()==QEvent::KeyPress)
        {
            QKeyEvent *event1 = static_cast<QKeyEvent *>(event);
            if (event1->key()==Qt::Key_Escape)
                mFundFlow.fundFlowChart->close();
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
            QPainter *painter=new QPainter(drawChart.hisTimeShareChartView);
            drawChart.drawHisTimeShare(painter);
            delete painter;
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
            else if (price<GlobalVar::hisPreClose)
                drawChart.hisTimeSharePrice->setStyleSheet("color:rgb(0,255,0);font:bold;font-size:18px");
            else
                drawChart.hisTimeSharePrice->setStyleSheet("color:yellow;font:bold;font-size:18px");
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
            isFlashBaseInfo=false;
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            flashOldCandleInfo(mouseEvent);
        }
        else if (event->type()==QEvent::MouseButtonDblClick)
        {
            if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or
                GlobalVar::curCode.left(2)=="10" or GlobalVar::WhichInterface==6 or
                GlobalVar::curCode.left(2)=="13" or GlobalVar::curCode.left(2)=="12")
            {
                QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
                return false;
            }
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (mouseEvent->pos().y()<30)
                return false;
            int m=(mouseEvent->pos().x()-KWIDTHEDGE)*GlobalVar::KRange/(drawChart.candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE);
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
            QPainter *painter=new QPainter(drawChart.candleChart);
            drawChart.drawCandleChart(painter);
            delete painter;
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
            isFlashBaseInfo=true;
            reFlashBuySellBaseInfo();
        }
        else if (event->type()==QEvent::ContextMenu)
        {
            QMenu *menu=new QMenu(drawChart.candleChart);
            menu->setAttribute(Qt::WA_DeleteOnClose);
            QAction *moveOne=new QAction("左右移动一格",menu);
            QAction *moveFast=new QAction("左右快速移动",menu);
            QIcon icon(":/new/png/png/step.jpeg");
            moveOne->setIcon(icon);
            QIcon icon1(":/new/png/png/fast.jpg");
            moveFast->setIcon(icon1);
            menu->addAction(moveFast);
            menu->addAction(moveOne);
            menu->popup(QCursor::pos());
            connect(moveOne,&QAction::triggered,this,[=](){
                drawChart.moveUnit=1;
            });
            connect(moveFast,&QAction::triggered,this,[=](){
                drawChart.moveUnit=GlobalVar::KRange*0.2;
            });
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
        for (int i=0;i<10;++i)
        {
            if (obj==buySellPrice[i] and (GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==4) and GlobalVar::curCode.left(1)!="1")
            {
                if (event->type()==QEvent::MouseButtonPress)
                    tradePrice=buySellPrice[i]->text().toFloat();
                else if (event->type()==QEvent::MouseMove and !buySellPrice[i]->text().isEmpty())
                    buySellPrice[i]->setStyleSheet("border-width:1px;border-style:solid;border-color:rgb(255,170,0);font:14px;font-family:微软雅黑;color:blue");
                else if (event->type()==QEvent::Leave)
                {
                    buySellPrice[i]->setStyleSheet("border-width: 0px;font:16px;font-family:微软雅黑");
                    float price=buySellPrice[i]->text().toFloat();
                    if (price>GlobalVar::preClose)
                        buySellPrice[i]->setPalette(GlobalVar::pRed);
                    else if (price<GlobalVar::preClose)
                        buySellPrice[i]->setPalette(GlobalVar::pGreen);
                    else
                        buySellPrice[i]->setPalette(GlobalVar::pBlack);
                }
            }
        }
        for (int i=0;i<50;++i)
            if (obj==drawChart.annLabel[i] and event->type()==QEvent::MouseButtonDblClick)
            {
                QString date=drawChart.annLabel[i]->toolTip().left(11);
                int curItem=0;
                QStringList l;
                bool f=true;
                for(int j = 0; j < GlobalVar::annoucementList.count()-1; j++)
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
                            GlobalVar::curName.left(GlobalVar::curName.indexOf("(")));
                drawChart.annClicked(qindex);
                drawChart.annoucementWindow->show();
                return true;
            }
    }
    return QMainWindow::eventFilter(obj, event);
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().ry()<250 and event->button()==Qt::RightButton)
        if (GlobalVar::WhichInterface==1 or (GlobalVar::WhichInterface==4 and GlobalVar::isKState))
            addRightMenu(3);
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QKeySequence key=event->key();
    if (key==Qt::Key_Escape)
    {
        if (GlobalVar::isKState)
        {
            if (not drawChart.annoucementWindow->isHidden())
                drawChart.annoucementWindow->close();
            else if (not drawChart.hisTimeShareChart->isHidden())
                drawChart.hisTimeShareChart->close();
            else
            {
                GlobalVar::isKState=false;
                isTraversalMyStock=false;
                if (GlobalVar::WhichInterface==1)
                    middleWindow->show();
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
        else if (GlobalVar::WhichInterface==1 or GlobalVar::WhichInterface==4)
            mFundFlow.fundFlowChart->close();
    }
    else if (key==Qt::Key_PageDown)
    {
        if (isThreadRunning)
            return;
        isThreadRunning=true;
        drawChart.hisTimeShareChart->close();
        drawChart.annoucementWindow->close();
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
                if (isTraversalMyStock)
                {
                    int curRow=mTableStock.myStockView->currentIndex().row()+1;
                    if (curRow>GlobalVar::mMyStockList.count()-1)
                        curRow=0;
                    GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
                    mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curRow,0));
                }
                else
                {
                    int curRow=mTableStock.stockTableView->currentIndex().row()+1;
                    if (curRow>GlobalVar::mTableList.count()-1)
                        curRow=0;
                    GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                    mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
                }
            }
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
            mFundFlow.initAllNews();
            drawChart.candleChart->update();
        }
        isThreadRunning=false;
    }
    else if (key==Qt::Key_PageUp)
    {
        if (isThreadRunning)
            return;
        isThreadRunning=true;
        drawChart.hisTimeShareChart->close();
        drawChart.annoucementWindow->close();
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
                if (isTraversalMyStock)
                {
                    int curRow=mTableStock.myStockView->currentIndex().row()-1;
                    if (curRow<0)
                        curRow=GlobalVar::mMyStockList.count()-1;
                    GlobalVar::curCode=GlobalVar::mMyStockList.at(curRow).code;
                    mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curRow,0));
                }
                else
                {
                    int curRow=mTableStock.stockTableView->currentIndex().row()-1;
                    if (curRow<0)
                        curRow=GlobalVar::mTableList.count()-1;
                    GlobalVar::curCode=GlobalVar::mTableList.at(curRow).code;
                    mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(curRow,0));
                }
            }
            emit startThreadCandleChart(freq,adjustFlag,true);
            emit startThreadTimeShareChart(false);
            emit startThreadTimeShareTick(false);
            mFundFlow.initAllNews();
            drawChart.candleChart->update();
        }
        isThreadRunning=false;
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
        if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or GlobalVar::WhichInterface==6)
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
            GlobalVar::offsetLocal=GlobalVar::offsetLocal+drawChart.moveUnit;
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
            GlobalVar::offsetLocal=GlobalVar::offsetLocal-drawChart.moveUnit;
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
    else if (key==Qt::Key_Delete and mTableStock.myStockView->hasFocus())
        delMyStock();
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    downUpLookStock(event);
}
void MainWindow::setMarket()
{
    timeCount=-3;
    QObject* obj = sender();
    if (obj->objectName()=="ZHMarket")
    {
        GlobalVar::WhichInterface=1;
        isAsia=true;
        ifCanClick=1;
        middleWindow->show();
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
        GlobalVar::curSortNum=6;
        isAsia=true;
        middleWindow->hide();
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
        GlobalVar::curSortNum=6;
        isAsia=false;
        GlobalVar::isUsZhStock=false;
        middleWindow->hide();
//        ui->HKMarket->setChecked(false);
//        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="105.NVDA";
        GlobalVar::curName="英伟达";
    }
    else if (obj->objectName()=="USzMarket")
    {
        saveCode();
        ifCanClick=-1;
        GlobalVar::WhichInterface=5;
        GlobalVar::curSortNum=6;
        isAsia=false;
        GlobalVar::isUsZhStock=true;
        middleWindow->hide();
//        ui->HKMarket->setChecked(false);
        ui->USMarket->setChecked(true);
//        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(true);
        GlobalVar::curCode="106.BABA";
        GlobalVar::curName="阿里巴巴";
    }
    else if (obj->objectName()=="UKMarket")
    {
        saveCode();
        ifCanClick=-1;
        GlobalVar::WhichInterface=6;
        GlobalVar::curSortNum=6;
        isAsia=false;
        middleWindow->hide();
        //        ui->HKMarket->setChecked(true);
        //        ui->USMarket->setChecked(false);
        //        ui->ZHMarket->setChecked(false);
        ui->USzMarket->setChecked(false);
        GlobalVar::curCode="155.SHEL";
        GlobalVar::curName="壳牌";
    }
    GlobalVar::isBoard=false;
    mTableStock.stockTableView->clearSpans();
    resetKParameter();
    toInterFace("main");
    emit startThreadTable();
    emit startThreadTimeShareChart(false);
    emit startThreadTimeShareTick(false);
    emit startThreadIndex();
}
void MainWindow::addRightMenu(int num)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction *act=new QAction("加入自选",menu);
    QIcon icon(":/new/png/png/join.jpg");
    act->setIcon(icon);
    menu->addAction(act);
    menu->popup(QCursor::pos());
    StockInfo info;
    QString code;
    if (num==1)
    {
        if (ifCanClick==1)
            info=GlobalVar::mTableList.at(mTableStock.stockTableView->currentIndex().row());
        else if (ifCanClick==2)
        {
            code=mFundFlow.model->item(mTableStock.stockTableView->currentIndex().row(),0)->text();
            info=GlobalVar::findStock(code);
        }
        else
            return;
    }
    else if(num==2)
        info=GlobalVar::mRisingSpeedList.at(mTableStock.risingSpeedView->currentIndex().row());
    else if(num==3)
    {
        code=GlobalVar::curCode;
        if (code.left(1)=="1" or code.left(3)=="399")
            return;
        info=GlobalVar::findStock(code);
    }
    connect(act,&QAction::triggered,this,[info, this](){
        for (int i=0;i<GlobalVar::mMyStockList.count();++i)
        {
            if (info.code==GlobalVar::mMyStockList.at(i).code)
                return;
        }
        GlobalVar::mMyStockList.append(info);
        GlobalVar::mMyStockCode.append(info.code);
        int curIndex=mTableStock.myStockView->currentIndex().row();
        mTableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList,false,false);
        mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curIndex,0));
        mTableStock.m_risingSpeedModel->setModelData(GlobalVar::mRisingSpeedList,false,true);
        if (ifCanClick==1)
            mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
        else
            mFundFlow.model->item(mTableStock.stockTableView->currentIndex().row(),1)->setForeground(QColor(255,140,0));
        GlobalVar::settings->setValue("myStock",GlobalVar::mMyStockCode);
    });

}
void MainWindow::showSearchResult()
{
    if (GlobalVar::isBoard)
    {
        resetKParameter();
        searchSmallWindow->hide();
        GlobalVar::WhichInterface=1;
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
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
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(false);
        toInterFace("k");
    }
}
void MainWindow::resetKParameter()
{
    GlobalVar::offsetEnd=GlobalVar::settings->value("offsetEnd").toInt();
    GlobalVar::KRange=KRANGE;
    GlobalVar::offsetLocal=GlobalVar::KRange;
    preCode="";
    isFlashBaseInfo=true;
}
void MainWindow::dealWithFundFlow()
{
    periodBox->setCurrentIndex(3);
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
void MainWindow::fastTrade()
{
    if ((GlobalVar::WhichInterface!=1 and GlobalVar::WhichInterface!=4) or GlobalVar::curCode.left(1)=="1" or tradePrice==0)
        return;
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction *actB=new QAction("闪电买入",menu);
    QIcon icon(":/new/png/png/buy.jpg");
    actB->setIcon(icon);
    menu->addAction(actB);
    QAction *actS=new QAction("闪电卖出",menu);
    QIcon icon1(":/new/png/png/sell.jpg");
    actS->setIcon(icon1);
    menu->addAction(actS);
    menu->popup(QCursor::pos());
    howPosition=0;

    connect(actB,&QAction::triggered,this,[=](){
        PyGILState_STATE state=PyGILState_Ensure();
//        PyObject* pModule = PyImport_ImportModule("qmt");
//        if(!pModule)
//            qDebug()<<"import failure";
        PyObject* pFunTrade = PyObject_GetAttrString(pModule,"getAsset");
//        if(!pFunTrade)
//            qDebug()<<"get function failed";
        PyObject* args = PyTuple_New(1);
        PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
        PyObject* outcome=PyObject_CallFunction(pFunTrade,"O",args);
        float cash=QString(PyUnicode_AsUTF8(outcome)).toFloat();
        PyGILState_Release(state);

        QDialog *fastBuy=new QDialog();
        fastBuy->setAttribute(Qt::WA_DeleteOnClose);
        fastBuy->setWindowFlags(fastBuy->windowFlags() | Qt::WindowStaysOnTopHint);
        fastBuy->setWindowTitle(GlobalVar::curName.split("(")[0]+" "+"闪电买入");
        fastBuy->setGeometry(670, 200, 260, 300);

        QLabel *infoName[5];
        QString name[5]={"证券代码:","报价方式:","买入价格:","最大可买:","买入数量:"};
        QVBoxLayout *mainLayout=new QVBoxLayout(fastBuy);
        QGridLayout *tradeInfo=new QGridLayout();
        QHBoxLayout *group=new QHBoxLayout();
        QHBoxLayout *buttom=new QHBoxLayout();

//        mainLayout->setContentsMargins(15, 5, 15, 20);
        mainLayout->addLayout(tradeInfo);
        mainLayout->addLayout(group);
        mainLayout->addSpacing(15);
        mainLayout->addLayout(buttom);
        for (int i=0;i<5;++i)
        {
            infoName[i]=new QLabel(name[i],fastBuy);
            infoName[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
            tradeInfo->addWidget(infoName[i],i,0);
        }
        QLabel *code=new QLabel(GlobalVar::curCode,fastBuy);
        QLabel *fix=new QLabel("限价委托",fastBuy);
        QDoubleSpinBox *price=new QDoubleSpinBox(fastBuy);
        QLabel *unit=new QLabel("元",fastBuy);
        QLabel *unit1=new QLabel("股",fastBuy);
        QLabel *unit2=new QLabel("股",fastBuy);

        QLabel *maxNumbers=new QLabel(GlobalVar::curCode,fastBuy);
        QSpinBox *buyNums=new QSpinBox(fastBuy);
        buyNums->setRange(0,1000000);
        buyNums->setSingleStep(100);
        int maxNums=floor(cash/tradePrice/100)*100;
        maxNumbers->setText(QString::number(maxNums));
        buyNums->setValue(maxNums);

        connect(price,&QDoubleSpinBox::valueChanged,this,[=](){
            int n=0;
            float m=price->text().toFloat();
            if (m)
                n=floor(cash/m/100);
            maxNumbers->setText(QString::number(n*100));
            buyNums->setValue(n/(howPosition+1)*100);
        });

        price->setSingleStep(0.01);
        price->setRange(0,10000);
        price->setValue(tradePrice);
        tradeInfo->addWidget(code,0,2);
        tradeInfo->addWidget(fix,1,2);
        tradeInfo->addWidget(price,2,2);
        tradeInfo->addWidget(unit,2,3);
        tradeInfo->addWidget(maxNumbers,3,2);
        tradeInfo->addWidget(unit1,3,3);
        tradeInfo->addWidget(buyNums,4,2);
        tradeInfo->addWidget(unit2,4,3);
        QButtonGroup *proportion=new QButtonGroup(fastBuy);
        QRadioButton *proportionName[5];
        QStringList proportionNums={"全仓","1/2","1/3","1/4","1/5"};
        for (int i=0;i<5;++i)
        {
            proportionName[i]=new QRadioButton(proportionNums[i],fastBuy);
            proportion->addButton(proportionName[i]);
            group->addWidget(proportionName[i]);
            connect(proportionName[i],&QRadioButton::clicked,this,[=]()mutable{
                int n=floor(cash/price->text().toFloat()/100);
                howPosition=i;
                buyNums->setValue(n/(i+1)*100);
            });
        }
        proportionName[0]->setChecked(true);
        QPushButton *buy=new QPushButton("买入",fastBuy);
        QPushButton *close=new QPushButton("取消",fastBuy);
        if (maxNums==0)
            buy->setEnabled(false);
        connect(buy,&QPushButton::clicked,this,[=](){
            PyGILState_STATE state=PyGILState_Ensure();
            // PyObject* pModule = PyImport_ImportModule("qmt");
            // if(!pModule)
            //     qDebug()<<"import failure";
            PyObject* pFunTrade = PyObject_GetAttrString(pModule,"qmtBuy");
            // if(!pFunTrade)
            //     qDebug()<<"get function failed";

            PyObject* args = PyTuple_New(4);
            QString code=GlobalVar::curCode;
            if (code.mid(0,1)=='3' or code.mid(0,1)=='0')
                code=code+".SZ";
            else
                code=code+".SH";
            PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
            PyTuple_SetItem(args,1,Py_BuildValue("s",code.toStdString().c_str()));
            PyTuple_SetItem(args,2,Py_BuildValue("i",buyNums->value()));
            PyTuple_SetItem(args,3,Py_BuildValue("f",price->value()));

            PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
            fastBuy->close();
            QMessageBox::information(this,"提示", QString(PyUnicode_AsUTF8(outcome)), QMessageBox::Ok);
            PyGILState_Release(state);
        });
        connect(close,&QPushButton::clicked,this,[=](){
            fastBuy->close();
        });

        buttom->addWidget(buy);
        buttom->addWidget(close);
        mainLayout->addSpacing(15);
        fastBuy->show();
    });
    connect(actS,&QAction::triggered,this,[=](){
        PyGILState_STATE state=PyGILState_Ensure();
//        PyObject* pModule = PyImport_ImportModule("qmt");
//        if(!pModule)
//            qDebug()<<"import failure";
        PyObject* pFunTrade = PyObject_GetAttrString(pModule,"getPositions");
//        if(!pFunTrade)
//            qDebug()<<"get function failed";
        PyObject* args = PyTuple_New(2);
        QString stockCode=GlobalVar::curCode;
        if (stockCode.mid(0,1)=='3' or stockCode.mid(0,1)=='0')
            stockCode=stockCode+".SZ";
        else
            stockCode=stockCode+".SH";
        PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
        PyTuple_SetItem(args,1,Py_BuildValue("s",stockCode.toStdString().c_str()));
        PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
        QStringList l=QString(PyUnicode_AsUTF8(outcome)).split(";");
        PyGILState_Release(state);
//        if (l[0]=="-1" or l[1]=="-1")
//            return;

        QDialog *fastSell=new QDialog();
        fastSell->setAttribute(Qt::WA_DeleteOnClose);
        fastSell->setWindowFlags(fastSell->windowFlags() | Qt::WindowStaysOnTopHint);
        fastSell->setWindowTitle(GlobalVar::curName.split("(")[0]+" "+"闪电卖出");
        fastSell->setGeometry(670, 200, 260, 300);

        QLabel *infoName[5];
        QString name[5]={"证券代码:","报价方式:","卖出价格:","最大可卖:","卖出数量:"};
        QVBoxLayout *mainLayout=new QVBoxLayout(fastSell);
        QGridLayout *tradeInfo=new QGridLayout();
        QHBoxLayout *group=new QHBoxLayout();
        QHBoxLayout *buttom=new QHBoxLayout();

        mainLayout->addLayout(tradeInfo);
        mainLayout->addLayout(group);
        mainLayout->addSpacing(15);
        mainLayout->addLayout(buttom);
        for (int i=0;i<5;++i)
        {
            infoName[i]=new QLabel(name[i],fastSell);
            infoName[i]->setStyleSheet("QLabel{font:16px;font-family:微软雅黑}");
            tradeInfo->addWidget(infoName[i],i,0);
        }
        QLabel *code=new QLabel(GlobalVar::curCode,fastSell);
        QLabel *fix=new QLabel("限价委托",fastSell);
        QDoubleSpinBox *price=new QDoubleSpinBox(fastSell);
        QLabel *unit=new QLabel("元",fastSell);
        QLabel *unit1=new QLabel("股",fastSell);
        QLabel *unit2=new QLabel("股",fastSell);

        QLabel *numbers=new QLabel(GlobalVar::curCode,fastSell);
        QSpinBox *sellNums=new QSpinBox(fastSell);
        sellNums->setRange(0,1000000);
        int maxNums=l[1].toInt();
        numbers->setText(QString::number(maxNums));
        sellNums->setValue(maxNums);

        price->setSingleStep(0.01);
        price->setRange(0,10000);
        price->setValue(tradePrice);
        tradeInfo->addWidget(code,0,2);
        tradeInfo->addWidget(fix,1,2);
        tradeInfo->addWidget(price,2,2);
        tradeInfo->addWidget(unit,2,3);
        tradeInfo->addWidget(numbers,3,2);
        tradeInfo->addWidget(unit1,3,3);
        tradeInfo->addWidget(sellNums,4,2);
        tradeInfo->addWidget(unit2,4,3);
        QButtonGroup *proportion=new QButtonGroup(fastSell);
        QRadioButton *proportionName[5];
        QStringList proportionNums={"全仓","1/2","1/3","1/4","1/5"};
        for (int i=0;i<5;++i)
        {
            proportionName[i]=new QRadioButton(proportionNums[i],fastSell);
            proportion->addButton(proportionName[i]);
            group->addWidget(proportionName[i]);
            connect(proportionName[i],&QRadioButton::clicked,this,[=](){
                sellNums->setValue(int(maxNums/(i+1)/100)*100);
            });
        }
        proportionName[0]->setChecked(true);
        QPushButton *sell=new QPushButton("卖出",fastSell);
        QPushButton *close=new QPushButton("取消",fastSell);
        if (maxNums==0)
            sell->setEnabled(false);
        connect(sell,&QPushButton::clicked,this,[=](){
            PyGILState_STATE state=PyGILState_Ensure();
            // PyObject* pModule = PyImport_ImportModule("qmt");
            // if(!pModule)
            //     qDebug()<<"import failure";
            PyObject* pFunTrade = PyObject_GetAttrString(pModule,"qmtSell");
            // if(!pFunTrade)
            //     qDebug()<<"get function failed";

            PyObject* args = PyTuple_New(4);
            QString code=GlobalVar::curCode;
            if (code.mid(0,1)=='3' or code.mid(0,1)=='0')
                code=code+".SZ";
            else
                code=code+".SH";
            PyTuple_SetItem(args,0,Py_BuildValue("s",account.toStdString().c_str()));
            PyTuple_SetItem(args,1,Py_BuildValue("s",code.toStdString().c_str()));
            PyTuple_SetItem(args,2,Py_BuildValue("i",sellNums->value()));
            PyTuple_SetItem(args,3,Py_BuildValue("f",price->value()));

            PyObject* outcome=PyObject_CallFunction(pFunTrade, "O",args);
            fastSell->close();
            QMessageBox::information(this,"提示", QString(PyUnicode_AsUTF8(outcome)), QMessageBox::Ok);
            PyGILState_Release(state);
        });
        connect(close,&QPushButton::clicked,this,[=](){
            fastSell->close();
        });

        buttom->addWidget(sell);
        buttom->addWidget(close);
        mainLayout->addSpacing(15);
        fastSell->show();
    });
}
void MainWindow::delMyStock()
{
    int curIndex=mTableStock.myStockView->currentIndex().row();
    if (curIndex==-1)
        return;
    GlobalVar::mMyStockList.removeAt(curIndex);
    GlobalVar::mMyStockCode.removeAt(curIndex);
    if (curIndex==mTableStock.m_myStockModel->rowCount()-1)
        curIndex-=1;
    mTableStock.m_myStockModel->setModelData(GlobalVar::mMyStockList,false,false);
    mTableStock.myStockView->setCurrentIndex(mTableStock.m_myStockModel->index(curIndex,0));
    mTableStock.m_risingSpeedModel->setModelData(GlobalVar::mRisingSpeedList,false,true);
    mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,false,true);
    GlobalVar::settings->setValue("myStock",GlobalVar::mMyStockCode);
}
void MainWindow::tradingTimeRunThread()
{
    QDateTime curTime=QDateTime::currentDateTime();
//    if (not ui->DLAllStockK->isEnabled() and curTime.time().toString("hh:mm")>"15:00")
//        ui->DLAllStockK->setEnabled(true);
    if (timeCount%2==0 and GlobalVar::WhichInterface==1 and GlobalVar::isZhMarketDay(curTime))
        if (GlobalVar::curCode.left(1)!="1" and GlobalVar::curCode.left(3)!="399")
            emit startThreadTimeShareTick(false);
    if (timeCount%6==1 and GlobalVar::WhichInterface==1)
    {
        if (GlobalVar::isZhMarketDay(curTime))
        {
            circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
            if (GlobalVar::isBoard)
                searchStock.getBoardData();
            emit startThreadTable();
            // if (GlobalVar::curCode.left(1)!="1" and GlobalVar::curCode.left(3)!="399")
            //     emit startThreadTimeShareChart(false);
        }
        else
            circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
    }
    else if (timeCount%10==4)
    {
        if (GlobalVar::isWorkDay(curTime))
            emit startThreadIndex();
        else
            reFlashIndex();
        if (GlobalVar::WhichInterface==5)
        {
            if (GlobalVar::isUSMarketDay(curTime))
            {
                circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
                emit startThreadTable();
            }
            else
                circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
        }
        else if (GlobalVar::WhichInterface==2)
        {
            if (GlobalVar::isHKMarketDay(curTime))
            {
                circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
                emit startThreadTable();
                emit startThreadTimeShareTick(false);
            }
            else
                circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
        }
        else if (GlobalVar::WhichInterface==6)
        {
            if (GlobalVar::isUKMarketDay(curTime))
            {
                circle->setStyleSheet(GlobalVar::circle_green_SheetStyle);
                emit startThreadTable();
            }
            else
                circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
        }
        else if (GlobalVar::WhichInterface==4)
            circle->setStyleSheet(GlobalVar::circle_red_SheetStyle);
    }
    else if (timeCount==30)
    {
        emit startThreadGetNews();
        QString d=curTime.date().toString("yyyy-MM-dd");
        if (d>downloadDate and GlobalVar::isWorkDay(curTime) and curTime.time().toString("hh:mm")>="09:00")
        {
            QStringList vacation=GlobalVar::settings->value("Vacation_ZH").toStringList();
            QString cur_date=curTime.toString("MMdd");
            if (not vacation.contains(cur_date))
            {
                requestsToCsv.downStockIndexPlateInfo();
                GlobalVar::settings->setValue("curTime",d);
                downloadDate=d;
                emit startThreadTimeShareChart(true);
                emit startThreadTimeShareTick(true);
            }
        }
        timeCount=0;
    }
    timeCount+=1;
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
    int m=20;
    n=19;
    if (changeInTurn)
    {
        n=18;
        m=21;
    }
    QPushButton *bb = (QPushButton *)(ui->statusBar->children().at(14));
    bb->setText(GlobalVar::mIndexList.at(n).name);
    QLabel *bl = (QLabel *)(ui->statusBar->children().at(15));
    if (GlobalVar::mIndexList.at(n).pctChg.toFloat()>0)
        bl->setPalette(GlobalVar::pRed);
    else if (GlobalVar::mIndexList.at(n).pctChg.toFloat()<0)
        bl->setPalette(GlobalVar::pGreen);
    else
        bl->setPalette(GlobalVar::pBlack);
    bl->setText(GlobalVar::mIndexList.at(n).close+" "+GlobalVar::mIndexList.at(n).pctChg+"%");
    bb = (QPushButton *)(ui->statusBar->children().at(16));
    bb->setText(GlobalVar::mIndexList.at(m).name);
    bl = (QLabel *)(ui->statusBar->children().at(18));
    if (GlobalVar::mIndexList.at(m).pctChg.toFloat()>0)
        bl->setPalette(GlobalVar::pRed);
    else if (GlobalVar::mIndexList.at(m).pctChg.toFloat()<0)
        bl->setPalette(GlobalVar::pGreen);
    else
        bl->setPalette(GlobalVar::pBlack);
    bl->setText(GlobalVar::mIndexList.at(m).close+" "+GlobalVar::mIndexList.at(m).pctChg+"%");
}
void MainWindow::reFlashBuySellBaseInfo()
{
    QString str;
    int d=GlobalVar::setRound();
    for (int i=0;i<10;++i)
    {
        float price=GlobalVar::buySellPrice[i];
        if (price==0)
            str="";
        else
        {
            str=QString::number(price,'f',d);
            if (price>GlobalVar::preClose)
                buySellPrice[i]->setPalette(GlobalVar::pRed);
            else if (price<GlobalVar::preClose)
                buySellPrice[i]->setPalette(GlobalVar::pGreen);
            else
                buySellPrice[i]->setPalette(GlobalVar::pBlack);
        }
        buySellPrice[i]->setText(str);
        int nums=GlobalVar::buySellNum[i];
        if (nums==0)
            str="";
        else
        {
            if (nums>1000000)
                str=GlobalVar::format_conversion(nums);
            else
                str=QString::number(nums);
        }
        buySellNum[i]->setText(str);
    }
    if (!isFlashBaseInfo)
        return;
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

    baseInfoData[0]->setText(QString::number(GlobalVar::baseInfoData[0],'f',d));
    for (int i=1;i<3;++i)
        baseInfoData[i]->setText(QString::number(GlobalVar::baseInfoData[i],'f',2)+"%");
    for (int i=3;i<7;++i)
        baseInfoData[i]->setText(GlobalVar::format_conversion(GlobalVar::baseInfoData[i]));
    float pct=0;
    for (int i=9;i<12;++i)
    {
        float v=GlobalVar::baseInfoData[i-2];
        if (GlobalVar::preClose==0 or v==0)
            pct=0;
        else
            pct=(v-GlobalVar::preClose)*100/GlobalVar::preClose;
        baseInfoData[i]->setText(QString::number(v,'f',d)+"("+GlobalVar::format_conversion(pct)+"%)");
        if (pct>0)
            baseInfoData[i]->setPalette(GlobalVar::pRed);
        else if(pct<0)
            baseInfoData[i]->setPalette(GlobalVar::pGreen);
        else
            baseInfoData[i]->setPalette(GlobalVar::pBlack);
    }
    for (int i=12;i<16;++i)
        if(i==14)
            baseInfoData[i]->setText(QString::number(GlobalVar::baseInfoData[i-2],'f',3));
        else
            baseInfoData[i]->setText(GlobalVar::format_conversion(GlobalVar::baseInfoData[i-2]));
    stockCode->setText(GlobalVar::curCode);
    stockName->setText(GlobalVar::curName);
    EPSLabel->setText(GlobalVar::EPSReportDate);
    PELabel->setText(GlobalVar::PEName);
}
void MainWindow::flashOldCandleInfo(QMouseEvent *mouseEvent)
{
    int m=(mouseEvent->pos().x()-KWIDTHEDGE)*GlobalVar::KRange/(drawChart.candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE);
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
    baseInfoData[12]->setPalette(GlobalVar::pBlue);
    baseInfoData[2]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).turn)+"%");
    baseInfoData[3]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).amount));
    baseInfoData[12]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).vol));
    if (GlobalVar::WhichInterface==1)
        baseInfoData[13]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).vol*10000/GlobalVar::mCandleChartList.at(n).turn));
    else
        baseInfoData[13]->setText(GlobalVar::format_conversion(GlobalVar::mCandleChartList.at(n).vol*100/GlobalVar::mCandleChartList.at(n).turn));
    if (n==0)
        temp=GlobalVar::mCandleChartList.at(n).close/(1+GlobalVar::mCandleChartList.at(n).pctChg/100);
    else
        temp=GlobalVar::mCandleChartList.at(n-1).close;
    QList<float> t={GlobalVar::mCandleChartList.at(n).open,GlobalVar::mCandleChartList.at(n).high,
                    GlobalVar::mCandleChartList.at(n).low,temp};
    for (int i=0;i<3;++i)
    {
        if (t[i]>t[3])
            baseInfoData[i+9]->setPalette(GlobalVar::pRed);
        else if (t[i]<t[3])
            baseInfoData[i+9]->setPalette(GlobalVar::pGreen);
        else
            baseInfoData[i+9]->setPalette(GlobalVar::pBlack);
        baseInfoData[i+9]->setText(QString::number(t[i])+"("+QString::number((t[i]-temp)*100/temp,'f',2)+"%)");
    }
    drawChart.rowTime->show();

    if (mouseEvent->pos().ry()>drawChart.candleChart->height()*12/15)
    {
        float vol=drawChart.candleHighLowPoint[2]-drawChart.candleHighLowPoint[2]*(mouseEvent->pos().ry()-drawChart.candleChart->height()*12/15-KBOTTOMHEIGHTEDGE)/(drawChart.candleChart->height()*3/15-2*KBOTTOMHEIGHTEDGE);
        drawChart.colPrice->setText(GlobalVar::format_conversion(int(vol)));
    }
    else
    {
        if (mouseEvent->pos().ry()>=KTOPHEIGHTEDGE)
        {
            float price=drawChart.candleHighLowPoint[0]-(drawChart.candleHighLowPoint[0]-drawChart.candleHighLowPoint[1])*(mouseEvent->pos().ry()-KTOPHEIGHTEDGE)/(drawChart.candleChart->height()*12/15-2*KTOPHEIGHTEDGE);
            drawChart.colPrice->setText(QString::number(price,'f',2));
        }
    }
    drawChart.colPrice->adjustSize();
    drawChart.rowTime->setText(GlobalVar::mCandleChartList.at(n).time);
    if (mouseEvent->pos().ry()>=KTOPHEIGHTEDGE)
    {
        drawChart.colPrice->show();
        drawChart.colPrice->move(drawChart.candleChart->width()-KRIGHTWIDTHEDGE,mouseEvent->pos().ry()-drawChart.colPrice->height());
        drawChart.hKLine->move(0,mouseEvent->pos().ry());
    }
    drawChart.rowTime->move(mouseEvent->pos().rx()-(mouseEvent->pos().x()-KWIDTHEDGE)*drawChart.rowTime->width()/(drawChart.candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE),drawChart.candleChart->height()*12/15);
    drawChart.vKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    drawChart.vKLine->resize(1,drawChart.candleChart->height());
    drawChart.hKLine->setStyleSheet("QLabel{border:2px dotted white;}");
    drawChart.hKLine->resize(drawChart.candleChart->width(),1);
    int posX=(2*m+1)*(drawChart.candleChart->width()-2*KWIDTHEDGE-KRIGHTWIDTHEDGE)/(2*GlobalVar::KRange);
    drawChart.vKLine->move(posX+KWIDTHEDGE,0);
    drawChart.vKLine->show();
    drawChart.hKLine->show();
}
void MainWindow::toInterFace(QString which)
{
    if (which=="main")
    {
        GlobalVar::isKState=false;
        isTraversalMyStock=false;
        rightFundWindow->hide();
        drawChart.candleChart->hide();
        rightBaseWindow->show();
        mTableStock.stockTableView->show();
        int row=mTableStock.stockTableView->currentIndex().row();
        if (row==-1)
            row=0;
        mTableStock.stockTableView->setModel(mTableStock.m_tableModel);
        mTableStock.m_tableModel->setModelData(GlobalVar::mTableList,true,true);
        mTableStock.stockTableView->setCurrentIndex(mTableStock.m_tableModel->index(row,0));
    }
    else if (which=="k")
    {
        GlobalVar::isKState=true;
        mTableStock.stockTableView->hide();
        middleWindow->hide();
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
        isTraversalMyStock=false;
        rightBaseWindow->hide();
        drawChart.candleChart->hide();
        middleWindow->hide();
        rightFundWindow->show();
        mTableStock.stockTableView->show();
//        mTableStock.stockTableView->setFocus();
    }
    else if(which=="f3")
    {
        if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or GlobalVar::WhichInterface==6)
        {
            QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
            return;
        }
        if (GlobalVar::curCode.left(1)=="1" or GlobalVar::curCode.left(3)=="399" or GlobalVar::curCode.left(3)=="899")
        {
            QMessageBox::information(this,"提示", "只能查看个股", QMessageBox::Ok);
            return;
        }
        F10SmallWindow->setFixedSize(675,500);
        fTitle->setText(GlobalVar::curCode+" "+GlobalVar::curName.left(GlobalVar::curName.indexOf("(")));
        F10SmallWindow->show();
        f10View.dealWithHotRank();
    }
    else if(which=="f10")
    {
        if (GlobalVar::curCode.left(1)=="1" or GlobalVar::curCode.left(3)=="399" or GlobalVar::curCode.left(3)=="899")
        {
            QMessageBox::information(this,"提示", "只能查看个股", QMessageBox::Ok);
            return;
        }
        if (GlobalVar::WhichInterface==2 or GlobalVar::WhichInterface==5 or GlobalVar::WhichInterface==6)
        {
            QMessageBox::information(this,"提示", "只能查看A股", QMessageBox::Ok);
            return;
        }
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
    mTableStock.stockTableView->clearSpans();
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
        mTableStock.stockTableView->setSpan(2,8,29,8);
        mTableStock.stockTableView->setIndexWidget(mFundFlow.model->index(2, 8), mFundFlow.tableChart);
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
    if (isThreadRunning)
        return;
    isThreadRunning=true;
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
            if (isTraversalMyStock)
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
            if (isTraversalMyStock)
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
        emit startThreadTimeShareChart(false);
        emit startThreadTimeShareTick(true);
        mFundFlow.initAllNews();
        drawChart.candleChart->update();
    }
    isThreadRunning=false;
}
