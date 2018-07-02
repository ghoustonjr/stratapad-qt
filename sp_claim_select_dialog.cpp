#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidgetItem>

#include "sp_claim_select_dialog.h"

ClaimSelectDialog::ClaimSelectDialog(spdb_manager* rm)
{
    setWindowTitle("Select claim");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_claimList = new QListWidget();
    populateClaimList(rm, m_claimList);

    m_ok = new QPushButton("Create Link");
    QPushButton* cancel = new QPushButton("Cancel");

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->addButton(m_ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancel, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(m_claimList);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    m_ok->setEnabled(false);

    connect(m_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_claimList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ClaimSelectDialog::validate);
}

void
ClaimSelectDialog::populateClaimList(spdb_manager* rm, QListWidget* list)
{

}

void
ClaimSelectDialog::validate()
{
    if (m_claimList->selectedItems().count() == 1)
    {
        m_ok->setEnabled(true);
    }
    else
    {
        m_ok->setEnabled(false);
    }
}

void
ClaimSelectDialog::accept()
{
    QList<QListWidgetItem *> selected = m_claimList->selectedItems();
    QListWidgetItem* item = selected.first();
    emit selectCitation(item->text());

    done(QDialog::Accepted);
}
