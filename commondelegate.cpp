
#include "commondelegate.h"

CommonDelegate::CommonDelegate()
{

}

void CommonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    itemOption.palette.setColor(QPalette::HighlightedText, index.data(Qt::ForegroundRole).value<QColor>());
    QStyledItemDelegate::paint(painter,itemOption,index);
}

