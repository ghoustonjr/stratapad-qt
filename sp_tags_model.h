#ifndef SP_TAGS_MODEL_H
#define SP_TAGS_MODEL_H

#include <QObject>
#include <QWidget>
#include <QAbstractListModel>

class sp_tags_model : public QAbstractListModel
{
public:
    explicit sp_tags_model(QObject *parent=0);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
};

#endif // SP_TAGS_MODEL_H
