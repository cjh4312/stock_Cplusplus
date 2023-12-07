#ifndef TABLESTOCK_H
#define TABLESTOCK_H

#include <QTableView>
#include <modeltablestock.h>
#include <QAbstractItemView>
#include <QScrollBar>
#include "modeltimeshare.h"
#include <QStandardItemModel>

class TableStock : public QTableView
{
public:
    TableStock();
    QTableView *stockTableView;
    QTableView *risingSpeedView;
    QTableView *myStockView;
    QTableView *timeShareTickView;
    ModelTableStock *m_tableModel;
    ModelTableStock *m_risingSpeedModel;
    ModelTableStock *m_myStockModel;
    ModelTimeShare *m_timeShareTickModel;
    void setTableView();
    void setTimeShareTickView();
    void setColumnWidth(QTableView *TV);

private:
    void initTableView();

};

#endif // TABLESTOCK_H
