
#include "modeltablestock.h"
#include "globalvar.h"
#include "qfont.h"

ModelTableStock::ModelTableStock(QObject *parent)
    : QAbstractTableModel(parent)
{
    tableHeader<<"代码"<<"名称"<<"最新价"<<"涨跌幅"<<"换手率"<<"成交额"<<"涨速"<<"市盈率"<<"总市值"<<"流通市值"<<"今年"<<"60日"<<"成交量"<< "最高"<< "最低"<< "今开"<<"昨收";
}

void ModelTableStock::setModelData(const QList<StockInfo> &data)
{
    beginResetModel();
    m_modelData = data;
    endResetModel();
}

int ModelTableStock::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_modelData.size();
}

int ModelTableStock::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return COLNUMS;
}

QVariant ModelTableStock::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) // 返回一个默认数据的表头
        return QAbstractTableModel::headerData(section, orientation, role);

    // 仅仅只是返回水平方向的表头
    if (orientation == Qt::Orientation::Horizontal) {
        return tableHeader[section];
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant ModelTableStock::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        //DisplayRole返回显示的文本值
        const int row = index.row();
        switch(index.column())
        {
        case 0: return m_modelData.at(row).code;
        case 1: return m_modelData.at(row).name;
        case 2: return m_modelData.at(row).close;
        case 3: return GlobalVar::format_conversion(m_modelData.at(row).pctChg)+"%";
        case 4: return m_modelData.at(row).turn;
        case 5: return GlobalVar::format_conversion(m_modelData.at(row).amount);
        case 6: return m_modelData.at(row).velocity;
        case 7: return m_modelData.at(row).pe;
        case 8: return GlobalVar::format_conversion(m_modelData.at(row).totalValue);
        case 9: return GlobalVar::format_conversion(m_modelData.at(row).circulatedValue);
        case 10: return m_modelData.at(row).pctYear;
        case 11: return m_modelData.at(row).pctSixty;
        case 12: return GlobalVar::format_conversion(m_modelData.at(row).volume);
        case 13: return m_modelData.at(row).high;
        case 14: return m_modelData.at(row).low;
        case 15: return m_modelData.at(row).open;
        case 16: return m_modelData.at(row).preClose;
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        const int row = index.row();
        switch(index.column())
        {
        case 2:
            if (m_modelData.at(row).close>m_modelData.at(row).open)
                return QColor(255, 0, 255);
            else if (m_modelData.at(row).close<m_modelData.at(row).open)
                return QColor(0, 191, 0);
            break;
        case 3:
            if (m_modelData.at(row).pctChg>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row).pctChg<0)
                return QColor(0, 191, 0);
            break;
        case 4:
            if (m_modelData.at(row).turn>=15)
                return QColor(204, 204, 0);
            break;
        case 5:
            if (m_modelData.at(row).amount>= 1000000000)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).amount >= 300000000)
                return QColor(0, 191, 255);
            break;
        case 6:
            if (m_modelData.at(row).velocity>= 2)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).velocity >0)
                return QColor(Qt::red);
            else if (m_modelData.at(row).velocity < 0)
                return QColor(0, 191, 0);
            break;
        case 10:
            if (m_modelData.at(row).pctYear >= 100)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).pctYear < 100)
                return QColor(255, 155, 153);
            else if (m_modelData.at(row).pctYear < 0)
                return QColor(0, 191, 0);
            break;
        case 11:
            if (m_modelData.at(row).pctSixty >= 100)
                return QColor(153, 0, 153);
            else if (m_modelData.at(row).pctSixty < 100)
                return QColor(255, 155, 153);
            else if (m_modelData.at(row).pctSixty < 0)
                return QColor(0, 191, 0);
            break;
        case 13:
            if (m_modelData.at(row).high>m_modelData.at(row).preClose)
                return QColor(Qt::red);
            else if (m_modelData.at(row).high<m_modelData.at(row).preClose)
                return QColor(0, 191, 0);
            break;
        case 14:
            if (m_modelData.at(row).high>m_modelData.at(row).preClose)
                return QColor(Qt::red);
            else if (m_modelData.at(row).high<m_modelData.at(row).preClose)
                return QColor(0, 191, 0);
            break;
        case 15:
            if (m_modelData.at(row).close>m_modelData.at(row).open)
                    return QColor(255, 0, 255);
            else if (m_modelData.at(row).close<m_modelData.at(row).open)
                    return QColor(0, 191, 0);
            break;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont boldFont = QFont();
        if (index.column() == 1)
            boldFont.setFamily("宋体");
        if (index.column() == 3 or index.column() == 1 or index.column() == 5)
        {
            boldFont.setBold(true);
            boldFont.setPixelSize(16);
            return boldFont;
        }
    }
    return QVariant();
}

