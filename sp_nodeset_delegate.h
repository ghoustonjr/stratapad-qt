#ifndef SP_NODESET_DELEGATE_H
#define SP_NODESET_DELEGATE_H

#include <QObject>
#include <QWidget>
#include <QStyledItemDelegate>

class sp_nodeset_delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    sp_nodeset_delegate(QWidget *parent=0) : QStyledItemDelegate(parent) {};

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif // SP_NODESET_DELEGATE_H
