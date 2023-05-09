
#ifndef F10VIEW_H
#define F10VIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include "getf10info.h"

class F10View : public QTableView
{
public:
    F10View();
    QTableView *stockInfoView;
    QStandardItemModel *model;
    GetF10Info getF10Info;
    void dealWithHotRank();
    void dealWithMainIndex();
    void dealWithBusinessAnalysis();
    void dealWithAssetLiability();
    void dealWithIncome();
    void dealWithCashFlow();

};

#endif // F10VIEW_H
