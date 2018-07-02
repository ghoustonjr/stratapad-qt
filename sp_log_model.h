#ifndef LOG_MODEL_H
#define LOG_MODEL_H

#include <QObject>
#include <QWidget>
#include <QAbstractListModel>

#include "sp_db_manager.h"

struct log_model_entry {
    int Timestamp;
    QString StateHash;
    QString Content;
};

struct test_log {
    QUuid Id;
    QString StateHash;
    QList<log_model_entry*> Entries;
};

class log_model : public QAbstractListModel {
    Q_OBJECT

public:
    explicit log_model(QObject* Parent = 0);

    QModelIndex index(int Row, int Column, const QModelIndex &Parent) const;
    int rowCount(const QModelIndex &Parent) const override;
    QVariant data(const QModelIndex &Index, int Role) const override;
    void setEntity(const QUuid& Entity);

    status_code CreateEntry(const QString& Contents);

    test_log *Log;
    spdb_manager *SPDBManager;

public slots:
    void SetSPDBManager(spdb_manager* Manager);
    void InformModelOfChanges(tx_report *TxReport);

signals:
    void EntryAdded(QString StateHash, int Timestamp, QString Content);
};

#endif // LOG_MODEL_H
