#include "sp_nodes_model.h"
#include "sp_nodeset_item.h"

int
sp_nodes_model::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant
sp_nodes_model::data(const QModelIndex &Index, int Role) const
{
    if (!Index.isValid())
    {
        return QVariant();
    }

    nodeset_item* Item = static_cast<nodeset_item *>(Index.internalPointer());

    if (Index.column() == 0)
    {
        if (Qt::DisplayRole == Role)
        {
            return Item->Title;
        }
        else if (Role == Qt::UserRole)
        {
            return Item->Id;
        }
        else if (Role == Qt::UserRole + 1)
        {
            return Item->Type;
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

QModelIndex
sp_nodes_model::IndexForUUID(const QUuid& Id)
{
    QModelIndexList indexes = match(index(0, 0, QModelIndex()), Qt::UserRole, Id, 1, (Qt::MatchStartsWith | Qt::MatchWrap | Qt::MatchRecursive));

    Q_ASSERT(!indexes.isEmpty());

    QModelIndex result = indexes.first();
    return result;
}

void
sp_nodes_model::SetSPDBManager(spdb_manager* Manager)
{
    beginResetModel();
    {
        this->Manager = Manager;
        m_items.clear();

        //TODO: Either free this memory when you're done, or create a functional interface for getting at it.
        node_summary *Summary = Sp_HeadCommitSummary(this->Manager->Repo);
        FOR_EACH(node_summary*, Entity, Summary)
        {
            nodeset_item *NewItem = new nodeset_item();

            NewItem->Hash = SpConv(Entity->Hash);
            NewItem->Id = SpConv(Entity->Id);
            NewItem->Title = (QString::fromUtf8((const char*)Entity->Title.Text, (int)Entity->Title.ByteSize));
            NewItem->Type = ObjectType_Node;

            m_items.append(NewItem);
        }
    }
    endResetModel();
    connect(&this->Manager->Signaler, &spdb_signaler::TxComplete, this, &sp_nodes_model::InformModelOfChanges);

    emit SPDBManagerReset();
}

void
sp_nodes_model::InformModelOfChanges(tx_report *TxReport)
{
    FOR_EACH(tx_report_link*, Link, &TxReport->Updated)
    {
        node Updated = {};
        QString HashString = SpConv(Link->Hash);
        status_code Result = NodeState(Manager, HashString, &Updated);
        Q_ASSERT(Result == StatusCode_OK);

        QModelIndex changed = IndexForUUID(SpConv(Updated.Id));
        nodeset_item* item = static_cast<nodeset_item *>(changed.internalPointer());
        item->Title = SpConv(Updated.Title);
        item->Hash = HashString;

        emit dataChanged(changed, changed);
    }

    FOR_EACH(tx_report_link*, Link, &TxReport->Added)
    {
        node Added = {};
        status_code Result = NodeState(Manager, SpConv(Link->Hash), &Added);
        Q_ASSERT(Result == StatusCode_OK);

        nodeset_item *Item = new nodeset_item();
        Item->Id = SpConv(Link->Id);
        Item->Hash = SpConv(Link->Hash);
        Item->Title = SpConv(Added.Title);

        beginInsertRows(QModelIndex(), 0, 0);
        m_items.append(Item);
        endInsertRows();
   }
}
