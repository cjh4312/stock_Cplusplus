#ifndef TABLESTOCK_H
#define TABLESTOCK_H

#include <QTableView>
#include <modeltablestock.h>
#include <QAbstractItemView>
#include <QScrollBar>
#include "modeltimeshare.h"

class TableStock : public QTableView
{
public:
    TableStock();
    QTableView *stockTableView;
    QTableView *risingSpeedView;
    QTableView *myStockView;
    QTableView *timeShareTickView;
    ModelTableStock *m_risingSpeedModel;
    ModelTableStock *m_myStockModel;
    ModelTimeShare *m_timeShareTickModel;
    void setTableView();
    void setTimeShareTickView();
    ModelTableStock *m_tableModel;
private:
    void initTableView();
    bool preSort=false;

};

#endif // TABLESTOCK_H
