#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QTableWidget>
#include <QGroupBox>

#include "sp_claim_view.h"
#include "sp_util.h"

ClaimView::ClaimView(QWidget* parent) : QWidget(parent)
{
    m_repoManager = 0;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QSplitter* splitter     = new QSplitter(Qt::Vertical);
    QTableWidget* evidence  = new QTableWidget();

    m_claim = new QTextBrowser();

    QFont font("Calibri");
    font.setStyleHint(QFont::SansSerif);
    font.setPointSize(12);
    m_claim->setFont(font);

    QGroupBox* claimBox         = new QGroupBox("Explanation");
    QVBoxLayout* claimBoxLayout = new QVBoxLayout();
    claimBoxLayout->addWidget(m_claim);
    claimBox->setLayout(claimBoxLayout);

    QGroupBox* evidenceBox         = new QGroupBox("Constraints");
    QVBoxLayout* evidenceBoxLayout = new QVBoxLayout();
    evidenceBoxLayout->addWidget(evidence);
    evidenceBox->setLayout(evidenceBoxLayout);

    splitter->addWidget(claimBox);
    splitter->setStretchFactor(0, 1);

    splitter->addWidget(evidenceBox);
    splitter->setStretchFactor(1, 3);

    splitter->setContentsMargins(0, 0, 0, 0);
    evidence->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(splitter);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}

void
ClaimView::setRepoManager(spdb_manager* rm)
{
    Q_ASSERT(rm);
    m_repoManager = rm;
}

void
ClaimView::setClaim(const QUuid& id)
{
    Q_ASSERT(m_repoManager);
    Q_ASSERT(!id.isNull());

    node_summary Summary = {};
    m_claim->setText(SpConv(Summary.Title));
}
