#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H
#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>

#include "sp_db_manager.h"

class search_dialog : public QDialog
{
    Q_OBJECT
public:
    search_dialog(QWidget* parent = 0);
    ~search_dialog();


    QPushButton* SearchButton;
    QTreeWidget* ResultsView;
    QLineEdit*   SearchTerms;
    QLabel*      ResultsCounter;

    spdb_manager* SPDBManager;

public slots:
    void BeginSearch();
    void ResetDialog();
    void SetSPDBManager(spdb_manager* NewManager);
};

#endif // SEARCHDIALOG_H
