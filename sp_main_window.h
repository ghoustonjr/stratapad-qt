#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTextEdit>
#include <QDir>
#include <QStackedWidget>
#include <QSortFilterProxyModel>
#include <QSettings>

#include "sp_nodeset_model.h"
#include "sp_search_dialog.h"
#include "sp_claim_dialog.h"
#include "sp_claim_select_dialog.h"
#include "sp_db_manager.h"
#include "sp_segmented_mode_control.h"
#include "sp_create_node_dialog.h"
#include "sp_richtext_editor.h"
#include "sp_nodes_model.h"

namespace Ui {
class MainWindow;
}

struct MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void MainWindowCenterAndResize();

public slots:

    void SPDBEnableEditActions(bool enabled);
    void SPDBOpenFromDialog();
    void SPDBCreate();
    void SPDBSetup(spdb_manager* rm);
    void SPDBCreateExplanation();
    void SPDBCreateObservation();

    void NodesetChangeSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void NodesetRenameItem();
    void NodesetDeleteItem();
    void NodesetCreateDialog();

    void WIPCommit();
    void WIPUpdate(bool ModifiedStatus);

    void StatusBarUpdate();
    void SearchShowDialog();
    void ConstraintShowSelectionDialog();

signals:
    void SPDBManagerChanged(spdb_manager *SPDBManager);
    void NodesModified(QList<QUuid> &NodeIds);

private:
    Ui::MainWindow *m_ui;
    QLabel *StatusBarLabel;
    search_dialog *SearchDialog;
    sp_segmented_mode_control *m_modeSelectionBar;

    spdb_manager *SPDBManager;
    sp_nodes_model *NodesetModel;
    QSortFilterProxyModel *NodesetProxyModel;

    sp_richtext_editor *SpRichtextEditor;

    QDir HomeDirectory;
    QUuid CurrentNodeId;

    QTimer *StatusBarTimer;
    QAction *UseEditingView;
    QAction *UseHistoryView;
    QAction *BoldTextAction;
    QAction *UnderlineTextAction;
    QAction *ItalicTextAction;
    QAction *AlignLeftAction;
    QAction *AlignCenterAction;
    QAction *AlignRightAction;
    QAction *AlignJustifyAction;
    QAction *UndoAction;
    QAction *RedoAction;
    QAction *RenameAction;
    QAction *DeleteAction;
    QAction *EditTagsAction;
    QAction *CreateExplAction;
    QAction *CreateObsAction;

    QList<QUuid> ModifiedNodeIds;
    QMap<QUuid, QTextDocument*> NodeContents;

    static const QString LastDbLocationSetting;
};

#endif // MAINWINDOW_H
