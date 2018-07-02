#include "sp_nodeset_delegate.h"
#include "sp_nodeset_item.h"

void sp_nodeset_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    if(index.isValid())
    {
        QVariant Type = index.model()->data(index, Qt::UserRole + 1);
        if(Type.isValid() && Type.canConvert(QVariant::Int))
        {

        }
    }

    QStyledItemDelegate::paint(painter, opt, index);
}
