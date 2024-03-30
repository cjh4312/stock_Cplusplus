
#include "modeltimeshare.h"
#include "globalvar.h"
#include "qfont.h"

ModelTimeShare::ModelTimeShare(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void ModelTimeShare::setModelData(const QList<timeShareTickInfo> &data)
{
    beginResetModel();
    m_modelData = data;
    endResetModel();
}

int ModelTimeShare::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_modelData.count();
}

int ModelTimeShare::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return col;
}

QVariant ModelTimeShare::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        //DisplayRole返回显示的文本值
        const int row = index.row();
        switch(index.column())
        {
        case 0: return m_modelData.at(row).time;
        case 1:
            {
                int p=2;
                if (GlobalVar::curCode.left(3)=="133")
                    p=4;
                return QString::number(m_modelData.at(row).price,'f',p);
            }
        case 2: return m_modelData.at(row).nums;
        case 3:
            if (m_modelData.at(row).d==1)
                return "S";
            else if (m_modelData.at(row).d==2)
                return "B";
            return "";
        case 4: return m_modelData.at(row).tick;
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        const int row = index.row();
        switch(index.column())
        {
        case 1:
            if (m_modelData.at(row).price>GlobalVar::preClose)
                return QColor(Qt::red);
            else if (m_modelData.at(row).price<GlobalVar::preClose)
                return QColor(0, 191, 0);
            else return QColor(Qt::black);
        case 2:
            return QColor(255,165,0);
        case 3:
            if (m_modelData.at(row).d==2)
                return QColor(Qt::red);
            else if(m_modelData.at(row).d==1)
                return QColor(0, 191, 0);
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch(index.column())
        {
//        case 0:return Qt::AlignCenter;
        case 1:return Qt::AlignRight;
        case 2:return Qt::AlignRight;
        case 4:return Qt::AlignRight;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont boldFont = QFont();
        if (index.column() == 3 or index.column() == 1 or index.column() == 2)
        {
            boldFont.setBold(true);
            boldFont.setPixelSize(16);
            return boldFont;
        }
    }
    return QVariant();
}

