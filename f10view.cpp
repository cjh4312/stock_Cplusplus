
#include "f10view.h"
#include "commondelegate.h"
#include "qheaderview.h"

F10View::F10View()
{
    stockInfoView=new QTableView(this);
    model = new QStandardItemModel(this);
    stockInfoView->setSelectionBehavior(QAbstractItemView::SelectRows);
    stockInfoView->setItemDelegate(new CommonDelegate());
    stockInfoView->setStyleSheet("QTableView{selection-background-color:lightgray}");
    stockInfoView->setAlternatingRowColors(true);
    stockInfoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPixelSize(16);
    stockInfoView->horizontalHeader()->setFont(boldFont);
}

void F10View::dealWithHotRank()
{
    getF10Info.getStockHotRank();
    model->clear();
    QStringList l;
    l<<"时间"<<"股票代码"<<"概念名称"<<"概念代码"<<"热度";
    model->setHorizontalHeaderLabels(l);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
    stockInfoView->setModel(model);
}

void F10View::dealWithMainIndex()
{
    getF10Info.getMainIndex();
    model->clear();

    model->setHorizontalHeaderLabels(getF10Info.colList);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
        {
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
            if (getF10Info.f10QList.at(i)[j].left(1)=="-")
                model->item(i,j)->setForeground(QColor(0, 191, 0));
        }
    stockInfoView->setModel(model);
    stockInfoView->setColumnWidth(0,200);
}

void F10View::dealWithBusinessAnalysis()
{
//    F10SmallWindow->setFixedSize(1500,700);
    getF10Info.mainBusinessComposition();
    model->clear();
    model->setHorizontalHeaderLabels(getF10Info.colList);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
        {
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
            if (getF10Info.f10QList.at(i)[j].left(1)=="-")
                model->item(i,j)->setForeground(QColor(0, 191, 0));
            else if (getF10Info.f10QList.at(i)[2]=="合计")
            {
                model->item(i,j)->setForeground(QColor(255, 0, 0));
                model->item(i,j)->setFont(QFont("微软雅黑",14,QFont::Black));
            }
            else if (getF10Info.f10QList.at(i)[1]=="按行业分")
            {
                model->item(i,j)->setForeground(QColor(255, 185, 15));
                model->item(i,j)->setFont(QFont("微软雅黑",12));
            }
            else if (getF10Info.f10QList.at(i)[1]=="按地区分")
            {
                model->item(i,j)->setForeground(QColor(Qt::blue));
                model->item(i,j)->setFont(QFont("微软雅黑",12));
            }
            else if (getF10Info.f10QList.at(i)[1]=="按产品分")
            {
                model->item(i,j)->setForeground(QColor(255, 0, 255));
                model->item(i,j)->setFont(QFont("微软雅黑",12));
            }
        }
    stockInfoView->setModel(model);
    stockInfoView->setColumnWidth(2,250);
    stockInfoView->setColumnWidth(3,120);
    stockInfoView->setColumnWidth(6,120);
}

void F10View::dealWithAssetLiability()
{
    getF10Info.assetLiability();
    model->clear();
    model->setHorizontalHeaderLabels(getF10Info.colList);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
        {
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
            if (getF10Info.f10QList.at(i)[j].left(1)=="-")
                model->item(i,j)->setForeground(QColor(0, 191, 0));
        }
    stockInfoView->setModel(model);
    stockInfoView->setColumnWidth(0,150);
}

void F10View::dealWithIncome()
{
    getF10Info.getIncome();
    model->clear();
    model->setHorizontalHeaderLabels(getF10Info.colList);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
        {
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
            if (getF10Info.f10QList.at(i)[j].left(1)=="-")
                model->item(i,j)->setForeground(QColor(0, 191, 0));
        }
    stockInfoView->setModel(model);
    stockInfoView->setColumnWidth(0,250);
}

void F10View::dealWithCashFlow()
{
    getF10Info.getCashFlow();
    model->clear();
    model->setHorizontalHeaderLabels(getF10Info.colList);
    for (int i=0;i<getF10Info.f10QList.size();++i)
        for (int j=0;j<getF10Info.f10QList.at(i).count();++j)
        {
            model->setItem(i, j, new QStandardItem(getF10Info.f10QList.at(i)[j]));
            if (getF10Info.f10QList.at(i)[j].left(1)=="-")
                model->item(i,j)->setForeground(QColor(0, 191, 0));
        }
    stockInfoView->setModel(model);
    stockInfoView->setColumnWidth(0,250);
}

