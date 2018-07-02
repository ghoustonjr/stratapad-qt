#ifndef CLAIMSELECTDIALOG_H
#define CLAIMSELECTDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QListWidget>

#include "sp_db_manager.h"

class ClaimSelectDialog : public QDialog {
    Q_OBJECT
public:
    ClaimSelectDialog(spdb_manager* rm);
    void populateClaimList(spdb_manager* rm, QListWidget* list);

    QPushButton* m_ok;
    QListWidget* m_claimList;

public slots:
    void validate();
    void accept() override;

signals:
    void selectCitation(const QString& claimSHA);
};

#endif // CLAIMSELECTDIALOG_H
