#include <QDesktopWidget>
#include <QDebug>
#include <QFileDialog>
#include <QToolButton>
#include <QDateTime>
#include <QTimer>
#include <QActionGroup>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QMessageBox>
#include <QTextBlockFormat>

#include "sp_util.h"
#include "sp_reader.h"
#include "sp_writer.h"
#include "sp_parser.h"
#include "sp_main_window.h"
#include "ui_mainwindow.h"
#include "sp_segmented_mode_control.h"
#include "sp_nodeset_delegate.h"

#include "model_test\modeltest.h"

const QString MainWindow::LastDbLocationSetting = QString("LastDbLocation");

enum {
    EditorStack_EDITOR = 0,
    EditorStack_MESSAGE,
    EditorStack_CLAIMVIEW
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    SPDBManager()
{
    m_ui->setupUi(this);

    this->SPDBManager = 0;


#ifdef QT_DEBUG
    QString path = QDir::home().absoluteFilePath("sp_test_repos");
    QDir home(path);
    if (!home.exists()) home.mkpath(".");

    HomeDirectory = home;
#else
    HomeDirectory = QDir::home();
#endif

    // TODO: This double widget-spacer thing is weird.
    QWidget* SpacerLeft  = new QWidget();
    QWidget* SpacerRight = new QWidget();
    SpacerLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    SpacerRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#if 0
    QToolBar* mainToolBar = this->addToolBar("Main Toolbar");
    mainToolBar->addAction(m_ui->actionCreate_Node);
    mainToolBar->addWidget(SpacerLeft);
    mainToolBar->addWidget(SpacerRight);
    mainToolBar->setFloatable(false);
    mainToolBar->setMovable(false);
#endif

    m_ui->editor_stack->setCurrentIndex(EditorStack_MESSAGE);

    m_ui->actionCreate_Node->setEnabled(false);
    m_ui->actionCommit_Changes->setEnabled(false);

    StatusBarLabel = new QLabel();
    m_ui->status_bar->addWidget(StatusBarLabel);

    StatusBarTimer = new QTimer(this);
    connect(StatusBarTimer, &QTimer::timeout, this, &MainWindow::StatusBarUpdate);
    StatusBarTimer->start(1000);

    SearchDialog = new search_dialog(this);
    SearchDialog->setWindowTitle("Search...");

    NodesetProxyModel = new QSortFilterProxyModel();
    NodesetProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    UseEditingView = new QAction(this);
    UseHistoryView = new QAction(this);

    UseEditingView->setText("Editing");
    UseHistoryView->setText("History");
    UseEditingView->setCheckable(true);
    UseHistoryView->setCheckable(true);

    QActionGroup* viewActions = new QActionGroup(this);
    viewActions->addAction(UseEditingView);
    viewActions->addAction(UseHistoryView);
    UseEditingView->setChecked(true);

    m_ui->menu_view->addAction(UseEditingView);
    m_ui->menu_view->addAction(UseHistoryView);

    m_ui->action_save_changes->setShortcut(QKeySequence::Save);

    CreateExplAction = new QAction(this);
    CreateExplAction->setText("Add Explanation...");

    CreateObsAction = new QAction(this);
    CreateObsAction->setText("Add Observation...");

    RenameAction = new QAction(this);
    RenameAction->setText("Rename...");
    connect(RenameAction, &QAction::triggered, this, &MainWindow::NodesetRenameItem);

    DeleteAction = new QAction(this);
    DeleteAction->setText("Delete");
    connect(DeleteAction, &QAction::triggered, this, &MainWindow::NodesetDeleteItem);

    EditTagsAction = new QAction(this);
    EditTagsAction->setText("Edit Tags...");
    connect(EditTagsAction, &QAction::triggered, [=](){qDebug() << "Trying to edit tags...";});

    BoldTextAction      = new QAction(this);
    ItalicTextAction    = new QAction(this);
    UnderlineTextAction = new QAction(this);

    BoldTextAction->setText("Bold");
    BoldTextAction->setShortcut(QKeySequence::Bold);
    ItalicTextAction->setText("Italic");
    ItalicTextAction->setShortcut(QKeySequence::Italic);
    UnderlineTextAction->setText("Underline");
    UnderlineTextAction->setShortcut(QKeySequence::Underline);

    m_ui->menu_format->addSection("Editing");
    m_ui->menu_format->addAction(BoldTextAction);
    m_ui->menu_format->addAction(ItalicTextAction);
    m_ui->menu_format->addAction(UnderlineTextAction);

    SpRichtextEditor = m_ui->note_editor->Editor;

    connect(m_ui->action_open_repo, &QAction::triggered, this, &MainWindow::SPDBOpenFromDialog);
    connect(m_ui->action_create_repo, &QAction::triggered, this, &MainWindow::SPDBCreate);
    connect(m_ui->action_create_expl, &QAction::triggered, this, &MainWindow::SPDBCreateExplanation);
    connect(m_ui->action_create_obs, &QAction::triggered, this, &MainWindow::SPDBCreateObservation);
    connect(m_ui->action_save_changes, &QAction::triggered, this, &MainWindow::WIPCommit);
    connect(m_ui->action_exit, &QAction::triggered, this, &MainWindow::close);

    connect(SpRichtextEditor, &sp_richtext_editor::AddCitation, this, &MainWindow::ConstraintShowSelectionDialog);

    m_ui->note_editor->blockSignals(true);

    // ==============================================
    // Setting up the NodesetModel and the nodes_view
    // ==============================================
    NodesetModel = new sp_nodes_model(this);
    new ModelTest(NodesetModel, this);
    NodesetProxyModel->setSourceModel(NodesetModel);

    m_ui->nodes_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->nodes_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_ui->nodes_view->setModel(NodesetModel);
    m_ui->nodes_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->nodes_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->nodes_view->setItemDelegate(new sp_nodeset_delegate(this));

    connect(m_ui->nodes_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::NodesetChangeSelection);

    // ================================================================
    // Everything that cares about the SPDBManager should register here
    // ================================================================
    connect(this, &MainWindow::SPDBManagerChanged, SearchDialog, &search_dialog::SetSPDBManager);
    connect(this, &MainWindow::SPDBManagerChanged, NodesetModel, &sp_nodes_model::SetSPDBManager);
    connect(this, &MainWindow::SPDBManagerChanged, m_ui->note_editor, &sp_node_editor::SetSPDBManager);

    QIcon filterIcon;
    filterIcon.addFile(":/icons/filter.png", QSize(32, 32), QIcon::Normal, QIcon::On);

    QSettings AppSettings;
    QString LastRepoLocation = AppSettings.value(MainWindow::LastDbLocationSetting).toString();
    if (!LastRepoLocation.isEmpty())
    {
        QFile File(LastRepoLocation);
        if(File.exists())
        {
            spdb_manager* Manager = OpenRepo(File);
            Q_ASSERT(Manager);

            // From AQP page 133: "[this code] is used when we want to invoke a slot via the event queue, so that invocation
            // takes place when the queue is empty, in this case, when the constructor has completed.
            QMetaObject::invokeMethod(this, "SPDBSetup", Qt::QueuedConnection, Q_ARG(spdb_manager *, Manager));
            QMetaObject::invokeMethod(this, "StatusBarUpdate", Qt::QueuedConnection);
        }

    }
}

MainWindow::~MainWindow()
{
    delete NodesetProxyModel;
    delete NodesetModel;
    delete SPDBManager;
    delete StatusBarLabel;
    delete StatusBarTimer;
    delete m_ui;
}

void
MainWindow::ConstraintShowSelectionDialog()
{
    ClaimSelectDialog d(SPDBManager);
    sp_richtext_editor* editor = qobject_cast<sp_richtext_editor *>(QObject::sender());
    Q_ASSERT(editor);

    connect(&d, &ClaimSelectDialog::selectCitation, editor, &sp_richtext_editor::CreateCitation);
    d.exec();
}

void
MainWindow::NodesetDeleteItem()
{
    QModelIndexList Selected       = m_ui->nodes_view->selectionModel()->selectedIndexes();
    QModelIndex SelectedIndexProxy = Selected.first();
    QModelIndex SelectedIndex      = NodesetProxyModel->mapToSource(SelectedIndexProxy);

    if (SelectedIndex.isValid())
    {
        nodeset_item* SelectedItem = static_cast<nodeset_item *>(SelectedIndex.internalPointer());
        switch(SelectedItem->Type)
        {
            case ObjectType_Node:
            {
                DeleteNode(SPDBManager, SelectedItem->Id);
            } break;

            InvalidDefaultCase;
        }
    }
}

void
MainWindow::NodesetRenameItem()
{
    QModelIndexList Selected       = m_ui->nodes_view->selectionModel()->selectedIndexes();
    QModelIndex SelectedIndexProxy = Selected.first();
    QModelIndex SelectedIndex      = NodesetProxyModel->mapToSource(SelectedIndexProxy);

    if (SelectedIndex.isValid())
    {
        m_ui->nodes_view->edit(SelectedIndexProxy);
    }
}

void
MainWindow::StatusBarUpdate()
{
    if (SPDBManager)
    {
        QString ElapsedMsg;

        s64 EpochalTime;
        HeadCommitTimestamp(SPDBManager, &EpochalTime);

        QDateTime Time = QDateTime::fromSecsSinceEpoch(EpochalTime);
        QDateTime Now  = QDateTime::currentDateTime();
        qint64 Elapsed = Time.secsTo(Now);

        if (Elapsed < 60)
        {
            ElapsedMsg = QString("(%1 seconds ago)").arg(QString::number(Elapsed));
        }
        else if (Elapsed < 3600)
        {
            int MinutesElapsed = Elapsed / 60;
            QString Plural = (MinutesElapsed == 1) ? "" : "s";
            ElapsedMsg = QString("(%1 minute%2 ago)").arg(QString::number(MinutesElapsed), Plural);
        }
        else if (Elapsed < (3600 * 24))
        {
            int HoursElapsed = (Elapsed / 60) / 60;
            ElapsedMsg = QString("(%1 hours ago)").arg(QString::number(HoursElapsed));
        }
        else
        {
            ElapsedMsg = "";
        }

        QString Message = "Last save: %1 %2";
        StatusBarLabel->setText(Message.arg(Time.toString("M/d/yy h:mm"), ElapsedMsg));
    }
    else
    {
        StatusBarLabel->setText("A repository has not been selected");
    }
}

void
MainWindow::MainWindowCenterAndResize()
{
    QSize AvailableSize = qApp->desktop()->availableGeometry().size();
    int Width  = AvailableSize.width();
    int Height = AvailableSize.height();

    Width  *= 0.75;
    Height *= 0.75;
    QSize NewSize(Width, Height);

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            NewSize,
            qApp->desktop()->availableGeometry()));
}

void
MainWindow::SPDBEnableEditActions(bool Enabled)
{
    m_ui->actionCommit_Changes->setEnabled(Enabled);
    m_ui->actionCreate_Node->setEnabled(Enabled);
}

void
MainWindow::SPDBOpenFromDialog(void)
{
    QString RepoDirectoryLocation = QFileDialog::getOpenFileName(this, tr("Open Repository"), HomeDirectory.absolutePath());
    if (!RepoDirectoryLocation.isEmpty())
    {
        spdb_manager* Manager = OpenRepo(QFile(RepoDirectoryLocation));
        if (Manager)
        {
            SPDBSetup(Manager);
        }
        else
        {
            QString ErrorMsg = QString("Failed to open the repo at %1").arg(RepoDirectoryLocation);
            QMessageBox::critical(this, "Critical!", ErrorMsg, QMessageBox::Ok, QMessageBox::NoButton);
            qCritical() << ErrorMsg;
        }
    }
}

void
MainWindow::SPDBCreateExplanation()
{

}

void
MainWindow::SPDBCreateObservation()
{

}

void
MainWindow::NodesetCreateDialog()
{
    CreateNodeDialog d;
    connect(&d, &CreateNodeDialog::NodeCreated, [ = ](basic_node Node){MakeNode(SPDBManager, Node);});
    d.exec();
}

void
MainWindow::SPDBCreate()
{
    QString Path = QFileDialog::getSaveFileName(this, tr("Create Repository"), HomeDirectory.absolutePath());
    Path.append(".spdb");
    QFile RepoFileName(Path);
    if (!Path.isEmpty())
    {
        spdb_manager* Manager = CreateRepo(RepoFileName);
        if (Manager)
        {
            SPDBSetup(Manager);
        }
        else
        {
            //TODO: Figure out how else to report this.
            QString ErrorMsg = QString("Failed attempting to create repo at %1").arg(Path);
            qDebug() << ErrorMsg;
            QMessageBox::critical(this, "Unable to create repo.", ErrorMsg, QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
    }
}

void
MainWindow::SPDBSetup(spdb_manager* NewManager)
{
    Q_ASSERT(NewManager);

    if (SPDBManager)
    {
        DELETE_AND_NULLIFY(SPDBManager);
    }

    SPDBManager = NewManager;
    QString SPDBPath = SPDBManager->DbFilePath;

    QSettings AppSettings;
    AppSettings.setValue(MainWindow::LastDbLocationSetting, SPDBPath);

    SPDBPath.append("[*]").append(" - Stratapad");
    setWindowTitle(SPDBPath);

    connect(&SPDBManager->Signaler, &spdb_signaler::TxComplete, this, &MainWindow::StatusBarUpdate);

    SPDBEnableEditActions(true);

    m_ui->note_editor->blockSignals(true);

    SpRichtextEditor->Clear();
    m_ui->editor_stack->setCurrentIndex(EditorStack_MESSAGE);

    emit SPDBManagerChanged(SPDBManager);
}

void
MainWindow::NodesetChangeSelection(const QItemSelection& selected, const QItemSelection&)
{
    if (!selected.indexes().isEmpty())
    {
        QModelIndex selectedIndexProxy = selected.indexes().first();
        QModelIndex selectedIndex      = NodesetProxyModel->mapToSource(selectedIndexProxy);
        nodeset_item* item = static_cast<nodeset_item *>(selectedIndex.internalPointer());

        if(item->Type == ObjectType_Node)
        {
            //TODO: Most of this, perhaps all of it, should move into the sp_node_editor class.
            node Node = {};
            status_code Result = NodeState(SPDBManager, item->Hash, &Node);
            Q_ASSERT(Result == StatusCode_OK);

            QUuid NodeId = SpConv(Node.Id);
            CurrentNodeId = item->Id;
            m_ui->editor_stack->setCurrentIndex(EditorStack_EDITOR);

            QTextDocument *Doc = 0;
            if (NodeContents.contains(NodeId))
            {
                Doc = NodeContents.value(NodeId);
            }
            else
            {
                Doc = new QTextDocument;

                QString Xml = SpConv(Node.Content);
                ReaderXML Reader(Xml);
                WriterMem Writer(Doc);
                StratapadParser Parser(&Reader, &Writer);
                Parser.parseDocument();

                QFont font("Century Schoolbook");
                font.setPointSize(12);
                Doc->setDefaultFont(font);

                QTextCursor Cursor(Doc);
                QTextBlockFormat Fmt = Cursor.blockFormat();
                Fmt.setTopMargin(14.0f);
                Fmt.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
                Cursor.setBlockFormat(Fmt);

                NodeContents.insert(NodeId, Doc);
                Doc->setMetaInformation(QTextDocument::DocumentTitle, NodeId.toString());
                Doc->clearUndoRedoStacks();
                Doc->setModified(false);
                connect(Doc, &QTextDocument::modificationChanged, this, &MainWindow::WIPUpdate);
            }
            Q_ASSERT(Doc);

            SpRichtextEditor->blockSignals(true);
            m_ui->note_editor->SetNode(item->Id, item->Hash);
            SpRichtextEditor->SwitchDoc(Doc);
            SpRichtextEditor->blockSignals(false);
        }
    }
}

void
MainWindow::WIPCommit()
{
    Q_ASSERT(!ModifiedNodeIds.empty());
    QMap<QString, QString> WIP;
    for(QUuid Id : ModifiedNodeIds)
    {
        QTextDocument *Doc = NodeContents.value(Id);
        Q_ASSERT(Doc);

        QModelIndex Index = NodesetModel->IndexForUUID(Id);
        nodeset_item *Item = static_cast<nodeset_item*>(Index.internalPointer());
        QString BasisHash = Item->Hash;

        ReaderMem t(Doc);
        WriterXML s;
        StratapadParser p(&t, &s);
        p.parseDocument();
        QString Content(*s.m_doc);

        WIP.insert(BasisHash, Content);
        Doc->clearUndoRedoStacks();
    }

    Q_ASSERT(!WIP.empty());

    CommitWIP(SPDBManager, WIP);

    ModifiedNodeIds.clear();
    setWindowModified(false);
    emit NodesModified(ModifiedNodeIds);
}

void
MainWindow::WIPUpdate(bool ModifiedStatus)
{
    Q_ASSERT(SPDBManager);

    QTextDocument* Doc = qobject_cast<QTextDocument*>(QObject::sender());
    Q_ASSERT(Doc);

    QUuid NodeId = QUuid(Doc->metaInformation(QTextDocument::DocumentTitle));
    Q_ASSERT(!NodeId.isNull());

    if(ModifiedStatus)
    {
        ModifiedNodeIds.append(NodeId);
    }
    else
    {
        ModifiedNodeIds.removeOne(NodeId);
    }

    setWindowModified(!ModifiedNodeIds.isEmpty());
    emit NodesModified(ModifiedNodeIds);
}

void
MainWindow::SearchShowDialog()
{
    SearchDialog->exec();
    SearchDialog->raise();
    SearchDialog->activateWindow();
}
