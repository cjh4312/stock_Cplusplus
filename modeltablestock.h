
#ifndef MODELTABLESTOCK_H
#define MODELTABLESTOCK_H

#include <QAbstractTableModel>
#include "stockinfo.h"

class ModelTableStock : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelTableStock(QObject *parent = nullptr);
    void setModelData(const QList<StockInfo> &data,bool forced,bool marking);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QStringList tableHeader;
    bool mark=true;

private:
//    QStringList tableList;
    QList<StockInfo> m_modelData;
};

#endif // MODELTABLESTOCK_H
