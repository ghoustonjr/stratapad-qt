#ifndef LOGEDITOR_H
#define LOGEDITOR_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QListView>
#include "sp_log_view.h"
#include "sp_log_model.h"
#include "sp_db_manager.h"
#include "sp_richtext_editor.h"

class LogEditor : public QWidget
{
    Q_OBJECT
public:
    explicit
    LogEditor(QWidget* parent = nullptr);

    log_model*       m_model;
    LogView*        m_logViewer;
    QListView*      m_entryList;
    sp_richtext_editor* m_editor;
    QPushButton*    m_addEntryButton;

public slots:
    void saveNewEntry();
    void initLogView();
    void changeLogEntrySelection(const QItemSelection& selected, const QItemSelection&);
};

#endif // LOGEDITOR_H
