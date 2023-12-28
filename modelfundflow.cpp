#include "modelfundflow.h"
#include "globalvar.h"
#include "qfont.h"

ModelFundFlow::ModelFundFlow(QObject *parent)
    : QAbstractTableModel{parent}
{
    tableHeader<<"名称"<<"涨跌幅"<<"主力净额"<<"主力净占比"<<"超大单净额"<<"超大单净占比"<<"大单净额"
                <<"大单净占比"<<"中单净额"<<"中单净占比"<<"小单净额"<<"小单净占比"<<"主力净流入最大股"<<"CODE";
}

void ModelFundFlow::setModelData(const QList<QStringList> &data,bool forced)
{
    if (m_modelData.size()!=data.size() or forced)
    {
        beginResetModel();
        m_modelData = data;
        endResetModel();
    }
    else
    {
        m_modelData = data;
        emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    }
}

int ModelFundFlow::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_modelData.size();
}

int ModelFundFlow::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 14;
}

QVariant ModelFundFlow::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) // 返回一个默认数据的表头
        return QAbstractTableModel::headerData(section, orientation, role);

    // 仅仅只是返回水平方向的表头
    if (orientation == Qt::Orientation::Horizontal) {
        return tableHeader[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant ModelFundFlow::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        //DisplayRole返回显示的文本值
        const int row = index.row();
        switch(index.column())
        {
        case 0: return m_modelData.at(row)[0];
        case 1: return m_modelData.at(row)[1]+"%";
        case 2: return GlobalVar::format_conversion(m_modelData.at(row)[2].toFloat());
        case 3: return m_modelData.at(row)[3];
        case 4: return GlobalVar::format_conversion(m_modelData.at(row)[4].toFloat());
        case 5: return m_modelData.at(row)[5];
        case 6: return GlobalVar::format_conversion(m_modelData.at(row)[6].toFloat());
        case 7: return m_modelData.at(row)[7];
        case 9: return m_modelData.at(row)[8];
        case 8: return m_modelData.at(row)[9];
        case 10: return m_modelData.at(row)[10];
        case 11: return m_modelData.at(row)[11];
        case 12: return m_modelData.at(row)[12];
        case 13: return m_modelData.at(row)[13];
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        const int row = index.row();
        switch(index.column())
        {
        case 0:
            return QColor(72,61,139);
        case 1:
            if (m_modelData.at(row)[1].toFloat()>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row)[1].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 4:
            if (m_modelData.at(row)[4].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 3:
            if (m_modelData.at(row)[3].toFloat()>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row)[3].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 5:
            if (m_modelData.at(row)[5].toFloat()>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row)[5].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 6:
            if (m_modelData.at(row)[6].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 2:
            if (m_modelData.at(row)[2].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 8:

            break;
        case 7:
            if (m_modelData.at(row)[7].toFloat()>0)
                return QColor(Qt::red);
            else if (m_modelData.at(row)[7].toFloat()<0)
                return QColor(0, 191, 0);
            break;
        case 9:

            break;
        case 10:

            break;
        case 11:

            break;
        case 13:

            break;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont boldFont = QFont();
        if (index.column() == 1)
            boldFont.setFamily("微软雅黑");
        if (index.column() == 0 or index.column() == 1)
        {
            boldFont.setBold(true);
            boldFont.setPixelSize(16);
            return boldFont;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch(index.column())
        {
        case 1:return Qt::AlignRight;
        case 2:return Qt::AlignCenter;
        case 3:return Qt::AlignCenter;
        case 4:return Qt::AlignCenter;
        case 5:return Qt::AlignCenter;
        case 6:return Qt::AlignCenter;
        }
    }
    return QVariant();
}

