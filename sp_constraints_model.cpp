#include "sp_constraints_model.h"

sp_constraints_model::sp_constraints_model(QObject *parent) : QAbstractTableModel(parent)
{
    HeaderTitles.append("Title");
    HeaderTitles.append("Tags");
    HeaderTitles.append("Status");
}

int sp_constraints_model::rowCount(const QModelIndex &parent) const
{
    return(Data.isEmpty() ? 0 : Data.count());
}

int sp_constraints_model::columnCount(const QModelIndex &parent) const
{
    QList<QVariant> Item = Data.first();
    return(Item.isEmpty() ? 0 : Item.count());
}

QVariant sp_constraints_model::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && (role == Qt::DisplayRole))
    {
        QList<QVariant> Row = Data.at(index.row());
        if(index.column() < Row.count()) {
            return(Row.at(index.column()));
        }
    }

    return QVariant();
}

QVariant sp_constraints_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role)
    {
        return(HeaderTitles.at(section));
    }

    return QVariant();
}
