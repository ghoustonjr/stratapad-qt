#ifndef CLAIMVIEW_H
#define CLAIMVIEW_H

#include <QObject>
#include <QWidget>
#include <QUuid>
#include <QTextBrowser>
#include "sp_db_manager.h"

class ClaimView : public QWidget
{
    Q_OBJECT
public:
    explicit
    ClaimView(QWidget* parent = nullptr);

    void setClaim(const QUuid& id);

    spdb_manager*  m_repoManager;
    QTextBrowser* m_claim;

signals:

public slots:
    void setRepoManager(spdb_manager* rm);
};

#endif // CLAIMVIEW_H
