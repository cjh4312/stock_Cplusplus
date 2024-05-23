
#ifndef MODELTIMESHARE_H
#define MODELTIMESHARE_H

#include <QAbstractTableModel>
#include "stockinfo.h"
#include "globalvar.h"
#include "qfont.h"

class ModelTimeShare : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModelTimeShare(QObject *parent = nullptr);
    void setModelData(const QList<timeShareTickInfo> &data);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    const int col=5;
    QList<timeShareTickInfo> m_modelData;
};

#endif // MODELTIMESHARE_H
