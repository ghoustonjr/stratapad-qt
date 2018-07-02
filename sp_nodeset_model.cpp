#include <QDebug>
#include <QMimeData>

#include "strata_shared.h"
#include "sp_util.h"
#include "sp_nodeset_model.h"

QString
formatContents(const QString& contents)
{
    QStringList split = contents.split("\n");
    return split.first();
}

sp_nodeset_model::sp_nodeset_model(QObject* parent) : QAbstractItemModel(parent)
{
    Manager = nullptr;
}

sp_nodeset_model::~sp_nodeset_model()
{
    if (TreeRoot)
    {
        delete TreeRoot;
    }
}

void
sp_nodeset_model::SetSPDBManager(spdb_manager* Manager)
{
    beginResetModel();
    {
        DELETE_AND_NULLIFY(TreeRoot);

        this->Manager = Manager;

        //TODO: Either free this memory when you're done, or create a functional interface for getting at it.
        node_summary *Summary = Sp_HeadCommitSummary(this->Manager->Repo);
        FOR_EACH(node_summary*, Entity, Summary)
        {
            nodeset_item *NewItem = 0;
            NewItem = new nodeset_item();

            NewItem->Type = Summary->Type;
            NewItem->Hash = SpConv(Entity->Hash);
            NewItem->Id = SpConv(Entity->Id);
            NewItem->Title = (QString::fromUtf8((const char*)Entity->Title.Text, Entity->Title.ByteSize));

            Items.append(NewItem);
        }
    }
    endResetModel();
    connect(&this->Manager->Signaler, &spdb_signaler::TxComplete, this, &sp_nodeset_model::InformModelOfChanges);

    emit SPDBManagerReset();
}

QModelIndex
sp_nodeset_model::index(int Row, int Column, const QModelIndex& ParentIndex) const
{
    if (!hasIndex(Row, Column, ParentIndex))
    {
        return QModelIndex();
    }

    return createIndex(Row, Column, (void *)Item);
}

QModelIndex
sp_nodeset_model::parent(const QModelIndex &Index) const
{
    if (!Index.isValid())
    {
        return QModelIndex();
    }

    nodeset_item* ChildItem  = static_cast<nodeset_item *>(Index.internalPointer());
    nodeset_item* ParentItem = ChildItem->Parent;

    if (ParentItem == TreeRoot)
    {
        return QModelIndex();
    }

    node_item_type NodeType = ParentItem->Type;
    int RowNumber = TreeRoot->Children.indexOf(ParentItem);

    Q_ASSERT(RowNumber >= 0);

    return createIndex(RowNumber, 0, (void *) ParentItem);
}

int
sp_nodeset_model::rowCount(const QModelIndex &Index) const
{
    if (Index.column() > 0)
    {
        return 0;
    }

    if (!Index.isValid())
    {
        return TreeRoot->Children.count();
    }

    nodeset_item* entity = static_cast<nodeset_item *>(Index.internalPointer());
    Q_ASSERT(entity);

    return entity->Children.count();
}

int
sp_nodeset_model::columnCount(const QModelIndex &Index) const
{
    Q_UNUSED(Index);
    return TreeRoot->Data.count();
}

QVariant
sp_nodeset_model::data(const QModelIndex &Index, int Role) const
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
            return Item->Data.value(0);
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

Qt::ItemFlags
sp_nodeset_model::flags(const QModelIndex& Index) const
{
    if (!Index.isValid())
    {
        return 0;
    }

    Qt::ItemFlags DefaultFlags = QAbstractItemModel::flags(Index);
    nodeset_item* Item = static_cast<nodeset_item *>(Index.internalPointer());

    switch (Item->Type)
    {
        case ObjectType_Node:
        {
            return Qt::ItemIsEditable | DefaultFlags;
        } break;

        default:
        {
            return DefaultFlags;
        } break;
    }
}

QVariant
sp_nodeset_model::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Qt::Horizontal == Orientation && Qt::DisplayRole == Role)
    {
        return TreeRoot->Data.at(Section);
    }

    return QVariant();
}

void
sp_nodeset_model::InformModelOfChanges(tx_report *TxReport)
{
    FOR_EACH(tx_report_link*, Link, &TxReport->Updated)
    {
        node Updated = {};
        QString HashString = SpConv(Link->Hash);
        status_code Result = NodeState(Manager, HashString, &Updated);
        Q_ASSERT(Result == StatusCode_OK);

        QModelIndex changed = IndexForUUID(SpConv(Updated.Id));
        nodeset_item* item = static_cast<nodeset_item *>(changed.internalPointer());
        item->Data.insert(0, SpConv(Updated.Title));
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
        Item->Data.append(SpConv(Added.Title));

        if(Added.Type == NodeType_Explanation)
        {
            int row = TreeRoot->Children.indexOf(ExplanationsRoot);
            beginInsertRows(index(row, 0), 0, 0);
            {
                Item->Parent = ExplanationsRoot;
                Item->Type = (node_item_type)ObjectType_Node;
                Item->Parent->Children.insert(0, Item);
            }
            endInsertRows();
        }
        else
        {
            int row = TreeRoot->Children.indexOf(ObservationsRoot);
            beginInsertRows(index(row, 0), 0, 0);
            {
                Item->Parent = ObservationsRoot;
                Item->Type = (node_item_type)ObjectType_Node;
                Item->Parent->Children.insert(0, Item);
            }
            endInsertRows();
        }
    }

    FOR_EACH(tx_report_link*, Link, &TxReport->Removed)
    {
        nodeset_item* Item = static_cast<nodeset_item *>(IndexForUUID(SpConv(Link->Id)).internalPointer());
        if (Item->IsExplanation)
        {
        Q_ASSERT(Item->Parent == ExplanationsRoot);
        int row = ExplanationsRoot->Children.indexOf(Item);

        // TODO: This doesn't make a lick of sense.
        QModelIndex Parent = this->parent(IndexForUUID(SpConv(Link->Id)));
        beginRemoveRows(Parent, row, row);
        {
            ExplanationsRoot->Children.removeAt(row);
        }
        endRemoveRows();
        }
        else
        {
        Q_ASSERT(Item->Parent == ObservationsRoot);
        int row = ObservationsRoot->Children.indexOf(Item);

        // TODO: This doesn't make a lick of sense.
        QModelIndex Parent = this->parent(IndexForUUID(SpConv(Link->Id)));
        beginRemoveRows(Parent, row, row);
        {
            ObservationsRoot->Children.removeAt(row);
        }
        endRemoveRows();
        }
    }
}

bool
sp_nodeset_model::setData(const QModelIndex &Index, const QVariant &Value, int Role)
{
    if ((Index.isValid()) && (Role == Qt::EditRole) && !Value.toString().isEmpty())
    {
        nodeset_item* Selected = static_cast<nodeset_item *>(Index.internalPointer());

        node Node = {};
        status_code Result = NodeState(Manager, Selected->Hash, &Node);
        Q_ASSERT(Result == StatusCode_OK);

        ChangeTitle(Manager, Selected->Hash, Value.toString());

        return true;
    }

    return false;
}

QModelIndex
sp_nodeset_model::IndexForUUID(const QUuid& Id)
{
    QModelIndexList indexes = match(index(0, 0, QModelIndex()), Qt::UserRole, Id, 1, (Qt::MatchStartsWith | Qt::MatchWrap | Qt::MatchRecursive));

    Q_ASSERT(!indexes.isEmpty());

    QModelIndex result = indexes.first();
    return result;
}

bool
sp_nodeset_model::insertRows(int row, int, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);
    nodeset_item* parentItem = static_cast<nodeset_item *>(parent.internalPointer());
    new nodeset_item(parentItem);
    endInsertRows();
    return true;
}

bool
sp_nodeset_model::removeRows(int row, int, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    nodeset_item* parentItem = static_cast<nodeset_item *>(parent.internalPointer());
    nodeset_item* deleting   = parentItem->Children.takeAt(row);
    delete deleting;
    endRemoveRows();
    return true;
}

QStringList
sp_nodeset_model::mimeTypes() const
{
    QStringList types;

    types << "application/x-note";

    return types;
}

QMimeData *
sp_nodeset_model::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* data = new QMimeData();

    QByteArray bytes;
    QDataStream dataStream(&bytes, QIODevice::WriteOnly);
    QModelIndex item   = indexes.first();
    nodeset_item* treeItem = static_cast<nodeset_item *>(item.internalPointer());

    dataStream << treeItem->Id;
    dataStream << treeItem->Data.first().toString();

    data->setData("application/x-note", bytes);
    return data;
}

bool
sp_nodeset_model::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column)

    if (!(Qt::IgnoreAction == action))
    {
        QDataStream dataStream(data->data("application/x-note"));
        QUuid entityID;
        QString title;
        dataStream >> entityID;
        dataStream >> title;

        row = rowCount(parent);

        qDebug() << "Failed changing the parent for note with ID:" << entityID;
    }

    return false;
}

Qt::DropActions
sp_nodeset_model::supportedDropActions() const
{
    return Qt::MoveAction;
}
