#ifndef SP_NODE_EDITOR_H
#define SP_NODE_EDITOR_H

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QTableView>
#include <QStringListModel>
#include <QSplitter>

#include "strata_shared.h"
#include "sp_db_manager.h"
#include "sp_flow_layout.h"
#include "sp_richtext_editor.h"
#include "sp_constraints_model.h"

class sp_node_editor : public QWidget
{
    Q_OBJECT
public:
    explicit sp_node_editor(QWidget *parent = nullptr);

    QVBoxLayout *MainLayout;
    QLabel *Title;
    sp_richtext_editor *Editor;
    QTableView *Links;
    sp_constraints_model *ConstraintsModel;
    spdb_manager *Manager;

    QUuid NodeId;
    QString NodeBasisHash;

signals:
    void CreatingNewTag();

public slots:
    void SetNode(const QUuid &Id, const QString &BasisHash);
    void SetSPDBManager(spdb_manager *Manager);
    void ProcessTxReport(tx_report *TxReport);
    void ShowNewTagDialog();
    void SetupTags(const QStringList &Tags);
};

#endif // SP_NODE_EDITOR_H
