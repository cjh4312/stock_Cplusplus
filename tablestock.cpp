#include "tablestock.h"
#include "commondelegate.h"
#include "qheaderview.h"
#include "globalvar.h"

TableStock::TableStock()
{
    stockTableView=new QTableView(this);
    blockView=new QTableView(this);
    risingSpeedView=new QTableView(this);
    myStockView=new QTableView(this);
    timeShareTickView=new QTableView(this);

    m_tableModel= new ModelTableStock(stockTableView);
    m_fundFlowModel=new ModelFundFlow(blockView);
    m_risingSpeedModel= new ModelTableStock(risingSpeedView);
    m_myStockModel= new ModelTableStock(myStockView);
    m_timeShareTickModel= new ModelTimeShare(timeShareTickView);
    initTableView();
}

void TableStock::setTableView()
{
    int row=stockTableView->currentIndex().row();
    if (row==-1)
        row=0;
    if (isFlashTable)
    {
        m_tableModel->setModelData(GlobalVar::mTableList,false,true);
        stockTableView->setCurrentIndex(m_tableModel->index(row,0));
    }
    if (GlobalVar::WhichInterface==1)
    {
        m_risingSpeedModel->setModelData(GlobalVar::mRisingSpeedList,false,true);
        m_myStockModel->setModelData(GlobalVar::mMyStockList,false,false);
    }
}

void TableStock::setColumnWidth(QTableView *TV)
{
    TV->setColumnWidth(0, 60);
    if (GlobalVar::WhichInterface==2 || GlobalVar::WhichInterface==5 || GlobalVar::WhichInterface==6)
        TV->setColumnWidth(1, 280);
    else
        TV->setColumnWidth(1, 75);
    TV->setColumnWidth(2, 50);
    TV->setColumnWidth(3, 66);
    TV->setColumnWidth(4, 75);
    TV->setColumnWidth(5, 60);
    TV->setColumnWidth(6, 85);
    TV->setColumnWidth(7, 80);
    TV->setColumnWidth(8, 70);
    TV->setColumnWidth(9, 90);
    TV->setColumnWidth(10, 65);
    TV->setColumnWidth(11, 65);
    TV->setColumnWidth(12, 70);
    TV->setColumnWidth(13, 70);
    TV->setColumnWidth(14, 70);
    TV->setColumnWidth(15, 70);
    TV->setColumnWidth(16, 70);
}
void TableStock::setTimeShareTickView()
{
    m_timeShareTickModel->setModelData(GlobalVar::mTimeShareTickList);
    timeShareTickView->scrollToBottom();
    timeShareTickView->setColumnWidth(0,60);
    timeShareTickView->setColumnWidth(1,85);
    timeShareTickView->setColumnWidth(2,75);
    timeShareTickView->setColumnWidth(3,12);
    timeShareTickView->setColumnWidth(4,48);
    blockView->setColumnWidth(0,135);
    blockView->setColumnWidth(1,78);
    blockView->setColumnWidth(3,85);
    blockView->setColumnWidth(4,50);
    blockView->setColumnWidth(6,50);
    blockView->setColumnWidth(7,85);
    blockView->setColumnWidth(8,50);
//    timeShareTickView->resizeColumnsToContents();

}

void TableStock::initTableView()
{
    stockTableView->setSortingEnabled(false);
    // 隐藏排序图标
    stockTableView->horizontalHeader()->setSortIndicatorShown(false);
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPixelSize(18);
    QHeaderView *header=myStockView->verticalHeader();
    //拖拽交换行
    header->setSectionsMovable(true);

    connect(header,&QHeaderView::sectionMoved,this,[=](int /*logicIndex*/,int oldIndex,int newIndex){
        StockInfo info=GlobalVar::mMyStockList.at(oldIndex);
        GlobalVar::mMyStockList.remove(oldIndex);
        GlobalVar::mMyStockList.insert(newIndex,info);
        QStringList s;
        for (int i=0;i<GlobalVar::mMyStockList.count();++i)
            s<<GlobalVar::mMyStockList.at(i).code;
        GlobalVar::settings->setValue("myStock",s);
        GlobalVar::mMyStockCode=s;
        QList<StockInfo> t;
        m_myStockModel->setModelData(t,false,false);
        m_myStockModel->setModelData(GlobalVar::mMyStockList,false,false);
        myStockView->setCurrentIndex(m_myStockModel->index(newIndex,0));
    });

    QTableView *tl[4]={stockTableView,risingSpeedView,myStockView,blockView};
    for (int i=0;i<4;++i)
    {
        tl[i]->horizontalHeader()->setSortIndicatorShown(false);
        tl[i]->horizontalHeader()->setSortIndicatorShown(false);
        tl[i]->horizontalHeader()->setFont(boldFont);
        tl[i]->verticalHeader()->setMinimumSectionSize(21);
        tl[i]->verticalHeader()->setDefaultSectionSize(21);
        tl[i]->setAlternatingRowColors(true);
        tl[i]->setShowGrid(false);
        tl[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        tl[i]->setSelectionMode(QAbstractItemView::SingleSelection);
        tl[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tl[i]->setStyleSheet("QTableView{selection-background-color:lightgray}");
        if (i==0)
            tl[0]->verticalScrollBar()->setStyleSheet("QScrollBar{width:10px;}");
        else
            tl[i]->verticalScrollBar()->setStyleSheet("QScrollBar{width:0px;}");
        tl[i]->horizontalScrollBar()->setStyleSheet("QScrollBar{width:0px;}");
        tl[i]->setItemDelegate(new CommonDelegate());
    }
    timeShareTickView->verticalHeader()->setVisible(false);
    timeShareTickView->horizontalHeader()->setVisible(false);
    timeShareTickView->setShowGrid(false);
    timeShareTickView->verticalHeader()->setMinimumSectionSize(18);
    timeShareTickView->verticalHeader()->setDefaultSectionSize(18);
    timeShareTickView->setSelectionMode(QAbstractItemView::NoSelection);
    timeShareTickView->horizontalHeader()->setMinimumSectionSize(1);
    timeShareTickView->setFrameStyle(0);
//    timeShareTickView->verticalScrollBar()->setStyleSheet("QScrollBar{width:15px;}");

    stockTableView->setModel(m_tableModel);
    risingSpeedView->setModel(m_risingSpeedModel);
    myStockView->setModel(m_myStockModel);
    blockView->setModel(m_fundFlowModel);
    timeShareTickView->setModel(m_timeShareTickModel);
    connect(m_tableModel, &ModelTableStock::dataChanged, stockTableView, [=](){update();});
    connect(m_risingSpeedModel, &ModelTableStock::dataChanged, risingSpeedView, [=](){update();});
    connect(m_myStockModel, &ModelTableStock::dataChanged, myStockView, [=](){update();});
    connect(m_fundFlowModel, &ModelTableStock::dataChanged, blockView, [=](){update();});
    connect(m_timeShareTickModel, &ModelTableStock::dataChanged, timeShareTickView, [=](){update();});
    connect(m_tableModel,&ModelTableStock::modelReset,stockTableView,[=](){
        setColumnWidth(stockTableView);
    });
    connect(m_risingSpeedModel,&ModelTableStock::modelReset,risingSpeedView,[=](){
        risingSpeedView->setCurrentIndex(m_risingSpeedModel->index(0,0));
        setColumnWidth(risingSpeedView);
    });
    connect(m_myStockModel,&ModelTableStock::modelReset,myStockView,[=](){
        myStockView->setCurrentIndex(m_myStockModel->index(0,0));
        setColumnWidth(myStockView);
    });
}
