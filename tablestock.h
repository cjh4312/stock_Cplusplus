#ifndef TABLESTOCK_H
#define TABLESTOCK_H

#include <QTableView>
#include <modeltablestock.h>
#include <QAbstractItemView>
#include <QScrollBar>
#include "modelfundflow.h"
#include "modeltimeshare.h"
#include <QStandardItemModel>

class TableStock : public QTableView
{
public:
    TableStock();
    QTableView *stockTableView;
    QTableView *blockView;
    QTableView *risingSpeedView;
    QTableView *myStockView;
    QTableView *timeShareTickView;
    ModelTableStock *m_tableModel;
    ModelTableStock *m_risingSpeedModel;
    ModelTableStock *m_myStockModel;
    ModelTimeShare *m_timeShareTickModel;
    ModelFundFlow *m_fundFlowModel;
    bool isFlashTable=true;
    QString preCode="";
    int preSize=0;
    void setTableView();
    void setTimeShareTickView();
    void setColumnWidth(QTableView *TV);
    void setBlockView();

private:
    void initTableView();

};

#endif // TABLESTOCK_H
