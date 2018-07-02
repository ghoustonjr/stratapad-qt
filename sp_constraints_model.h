#ifndef SP_CONSTRAINTS_MODEL_H
#define SP_CONSTRAINTS_MODEL_H

#include <QAbstractTableModel>

class sp_constraints_model : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit sp_constraints_model(QObject *parent=0);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QList<QList<QVariant>> Data;
    QList<QString> HeaderTitles;
};

#endif // SP_CONSTRAINTS_MODEL_H
