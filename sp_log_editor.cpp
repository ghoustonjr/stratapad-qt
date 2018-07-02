#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDateTime>
#include <QShortcut>
#include <QKeySequence>
#include <QAction>
#include <QGroupBox>
#include <QTextBlock>
#include <QTextFragment>
#include <QDebug>
#include <QTextDocumentFragment>
#include <QLineEdit>

#include "sp_log_editor.h"
#include "sp_util.h"

LogEditor::LogEditor(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    mainLayout->setMargin(0);
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(0);

    mainLayout->addWidget(mainSplitter);

    m_entryList = new QListView(this);
    mainSplitter->addWidget(m_entryList);

    QWidget* logViewAndEditorWidget = new QWidget();
    mainSplitter->addWidget(logViewAndEditorWidget);

    mainSplitter->setStretchFactor(1, 2);

    QVBoxLayout* viewAndEditorLayout = new QVBoxLayout();
    viewAndEditorLayout->setMargin(0);
    viewAndEditorLayout->setSpacing(0);
    logViewAndEditorWidget->setLayout(viewAndEditorLayout);

    QSplitter* viewAndEditorSplitter = new QSplitter(Qt::Vertical, this);

    m_logViewer      = new LogView(this);
    m_editor         = new sp_richtext_editor(this);
    m_addEntryButton = new QPushButton("Save Entry");

    QAction* addEntry = new QAction(this);
    QKeySequence seq  = QKeySequence(Qt::CTRL + Qt::Key_Return);
    addEntry->setShortcut(seq);
    addEntry->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    addAction(addEntry);

    m_addEntryButton->setToolTip("Add new scratch log entry " + seq.toString());

    viewAndEditorSplitter->addWidget(m_logViewer);
    viewAndEditorSplitter->setStretchFactor(0, 2);
    viewAndEditorSplitter->addWidget(m_editor);
    viewAndEditorSplitter->setHandleWidth(0);

    viewAndEditorLayout->addWidget(viewAndEditorSplitter);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(5);
    buttonLayout->setContentsMargins(5, 5, 5, 5);
    m_addEntryButton->setFixedHeight(20);
    buttonLayout->addWidget(m_addEntryButton);
    buttonLayout->addStretch(1);
    viewAndEditorLayout->addLayout(buttonLayout);

    m_editor->SetAcceptRichText(false);
    m_logViewer->layout()->setContentsMargins(0, 0, 0, 0);

    m_model = new log_model(this);
    m_entryList->setModel(m_model);

    connect(m_addEntryButton, &QPushButton::pressed, addEntry, &QAction::trigger);
    connect(addEntry, &QAction::triggered, this, &LogEditor::saveNewEntry);
    connect(m_model, &log_model::EntryAdded, m_logViewer, &LogView::addLogEntry);
    connect(m_model, &log_model::modelReset, this, &LogEditor::initLogView);
    connect(m_entryList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &LogEditor::changeLogEntrySelection);
}

void
LogEditor::initLogView()
{
    m_logViewer->m_log->clear();
    if(m_model->Log)
    {
        for (log_model_entry* entry : m_model->Log->Entries)
        {
            m_logViewer->addLogEntry(entry->StateHash, entry->Timestamp, entry->Content);
        }
    }
}

void
LogEditor::saveNewEntry()
{
    QString contents = m_editor->XML();

    if (m_model->CreateEntry(contents))
    {
        m_editor->Clear();
    }
    else
    {
        qDebug() << "Failed to add the entry to the scratch log.";
    }
}

void
LogEditor::changeLogEntrySelection(const QItemSelection& selected, const QItemSelection&)
{
    if (!selected.indexes().isEmpty())
    {
        QModelIndex selectedIndex = selected.indexes().first();
        log_model_entry* entry = static_cast<log_model_entry *>(selectedIndex.internalPointer());

        m_logViewer->m_log->scrollToAnchor(entry->StateHash);
    }
}
