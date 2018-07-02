#ifndef SP_NODES_MODEL_H
#define SP_NODES_MODEL_H

#include <QAbstractListModel>
#include <QUuid>

#include "strata_shared.h"
#include "sp_db_manager.h"
#include "sp_nodeset_item.h"

class sp_nodes_model : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit sp_nodes_model(QObject *parent=0) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex IndexForUUID(const QUuid& id);

    QList<nodeset_item*> m_items;

public slots:
    void SetSPDBManager(spdb_manager* rm);
    void InformModelOfChanges(tx_report *t);

signals:
    void SPDBManagerReset();

private:
    spdb_manager* Manager;

};

#endif // SP_NODES_MODEL_H
