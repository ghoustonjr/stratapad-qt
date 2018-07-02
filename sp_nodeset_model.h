#ifndef SP_NODESETMODEL_H
#define SP_NODESETMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QDir>

#include "sp_db_manager.h"
#include "sp_nodeset_item.h"

struct sp_nodeset_model : public QAbstractListModel {
    Q_OBJECT

public:
    explicit
    sp_nodeset_model(QObject* parent = 0);
    ~sp_nodeset_model();

    // QUERY
    QModelIndex      index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex      parent(const QModelIndex& index) const override;
    QVariant         data(const QModelIndex& index, int role) const override;
    int              rowCount(const QModelIndex& index) const override;
    int              columnCount(const QModelIndex& index) const override;
    Qt::ItemFlags    flags(const QModelIndex& index) const override;
    QVariant         headerData(int section, Qt::Orientation orientation, int role) const override;

    // COMMAND
    bool             setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool             insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool             removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // DRAG AND DROP
    QStringList     mimeTypes() const override;
    QMimeData *     mimeData(const QModelIndexList &indexes) const override;
    bool            dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::DropActions supportedDropActions() const override;

    QModelIndex IndexForUUID(const QUuid& id);

public slots:
    void SetSPDBManager(spdb_manager* rm);
    void InformModelOfChanges(tx_report *t);

signals:
    void SPDBManagerReset();

private:
    spdb_manager* Manager;
    QList<nodeset_item*> Items;
};

#endif
