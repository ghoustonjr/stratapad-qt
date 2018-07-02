#ifndef SPDB_SIGNALER_H
#define SPDB_SIGNALER_H

#include <QObject>
#include "sp_types.h"

class spdb_signaler : public QObject
{
    Q_OBJECT

signals:
    void    TxComplete(tx_report *t);
    void    WIPNew(const QUuid& noteID);
    void    WIPCleared();

public slots:
};

#endif // SPDB_SIGNALER_H
