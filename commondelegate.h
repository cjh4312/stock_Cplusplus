
#ifndef COMMONDELEGATE_H
#define COMMONDELEGATE_H

#include <QStyledItemDelegate>



class CommonDelegate : public QStyledItemDelegate
{
public:
    CommonDelegate();
private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // COMMONDELEGATE_H
