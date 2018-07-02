#ifndef REPOMANAGER_H
#define REPOMANAGER_H

#include <QUuid>
#include <QList>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTextDocument>

#include "sp_types.h"
#include "sp_db_signaler.h"

#include "strata_shared.h"

struct spdb_manager {
    repository *Repo;
    QString DbFilePath;
    spdb_signaler Signaler;
};

void InitBackend();

spdb_manager* CreateRepo(const QFile &path);
spdb_manager* OpenRepo(const QFile &path);
status_code NodeContent(spdb_manager *Manager, const QString& CommitSha, QString *Out);
status_code NodeState(spdb_manager *Manager, const QString& NodeStateSha, node *Out);
status_code HeadCommitTimestamp(spdb_manager *Manager, s64 *Out);
status_code Transact(spdb_manager *Manager);
status_code MakeNode(spdb_manager *Manager, basic_node Node);
status_code ChangeTitle(spdb_manager *Manager, const QString &BasisHash, const QString &Title);
status_code DeleteNode(spdb_manager *Manager, const QUuid &NodeId);
status_code CommitWIP(spdb_manager *Manager, const QMap<QString, QString> &WIP);
status_code AddTag(spdb_manager *Manager, const QUuid &Id, const QString &Tag);

Q_DECLARE_METATYPE(spdb_manager *)

#endif
