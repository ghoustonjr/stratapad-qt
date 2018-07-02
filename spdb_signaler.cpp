#include "spdb_signaler.h"

void
spdb_signaler::transactionComplete(tx_report *t)
{
    emit this->qtTransactionComplete(t);
}

void
spdb_signaler::noteHasWorkInProgress(QUuid noteId)
{
    emit this->qtNoteHasWorkInProgress(noteId);
}

void
spdb_signaler::workInProgressCleared()
{
    emit this->qtWorkInProgressCleared();
}
