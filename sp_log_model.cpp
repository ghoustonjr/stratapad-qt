#include "sp_log_model.h"
#include "sp_util.h"

log_model::log_model(QObject* parent) : QAbstractListModel(parent)
{
    Log = 0;
}

int
log_model::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (Log)
    {
        return Log->Entries.count();
    }

    return 0;
}

QModelIndex
log_model::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    log_model_entry* item;
    item = Log->Entries.at(row);

    return createIndex(row, column, (void *) item);
}

QVariant
log_model::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && Log && (index.row() <= Log->Entries.count()))
    {
        if (role == Qt::DisplayRole)
        {
            qint64 time = Log->Entries.at(index.row())->Timestamp;
            return QDateTime::fromSecsSinceEpoch(time).toString();
        }
    }

    return QVariant();
}

void
log_model::SetSPDBManager(spdb_manager* rm)
{
    Q_ASSERT(rm);

    beginResetModel();
    {
        if (Log)
        {
            DELETE_AND_NULLIFY(Log);
        }

        SPDBManager = rm;
    }
    endResetModel();
    connect(&rm->Signaler, &spdb_signaler::TxComplete, this, &log_model::InformModelOfChanges);
} // LogModel::setRepoManager

#if 0
void
LogModel::setEntity(const QUuid &entity)
{
    Q_ASSERT(m_repoManager);
    beginResetModel();
    {
        if (m_log)
        {
            DELETE_AND_NULLIFY(m_log);
        }

        m_log     = new Log;
        m_log->id = entity;
        m_log->currentStateSHA = m_repoManager->entityStateSHA(m_log->id);

        EntityState logState = m_repoManager->entityState(m_log->id);
        for (QString logEntryStateSHA : logState.links)
        {
            LogEntry fullEntry;
            ObjectMap object = m_repoManager->m_objectCache.value(logEntryStateSHA);

            fullEntry.timestamp = object.value("timestamp").toInt();
            fullEntry.contents  = object.value("contents").toString();

            m_log->entries.append(fullEntry);
        }
    }
    endResetModel();
}

#endif // if 0

status_code
log_model::CreateEntry(const QString &Contents)
{
    Q_UNUSED(Contents);
    NotYetImplemented;
    return StatusCode_ERROR;
}

void
log_model::InformModelOfChanges(tx_report *TxReport)
{
    if (Log)
    {
        FOR_EACH(tx_report_link*, Link, &TxReport->Added)
        {
            if (SpConv(Link->Id) == Log->Id)
            {
                // There's only one kind of change I'm interested in, and that's the addition of a log entry.
                // Have to 1) add that new entry to the list and 2) figure out what state the log itself is now in.

                int first = Log->Entries.count();
                int last  = first;
                beginInsertRows(QModelIndex(), first, last);
                {
                }
                endInsertRows();
            }
        }
    }
} // LogModel::informModelOfChanges
