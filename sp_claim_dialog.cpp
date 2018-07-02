#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>

#include "sp_claim_dialog.h"

ClaimDialog::ClaimDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle("Create New Explanation");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout;

    m_topicIn   = new QComboBox;
    m_contentIn = new QPlainTextEdit;

    m_newTopicLabel = new QLabel("New Topic");
    m_newTopicIn    = new QLineEdit();
    m_titleIn       = new QLineEdit();

    m_topicSelect = new QWidget();
    QVBoxLayout* topicSelectLayout = new QVBoxLayout;

    m_addTopic = new QWidget();
    m_addTopic->setVisible(false);
    QHBoxLayout* addTopicLayout = new QHBoxLayout;

    topicSelectLayout->setMargin(0);
    topicSelectLayout->addWidget(m_topicIn);

    addTopicLayout->setMargin(0);
    addTopicLayout->addWidget(m_newTopicLabel);
    addTopicLayout->addWidget(m_newTopicIn);

    m_addTopic->setLayout(addTopicLayout);
    m_topicSelect->setLayout(topicSelectLayout);

    formLayout->addRow("Topic", m_topicSelect);
    formLayout->addRow("Title", m_titleIn);
    formLayout->addRow("Explanation", m_contentIn);

    m_ok = new QPushButton("Create Explanation");
    m_ok->setDisabled(true);
    QPushButton* cancel = new QPushButton("Cancel");

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->addButton(m_ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancel, QDialogButtonBox::RejectRole);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    connect(m_topicIn, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ClaimDialog::validateForm);
    connect(m_contentIn, &QPlainTextEdit::textChanged, this, &ClaimDialog::validateForm);
    connect(m_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
}

void
ClaimDialog::validateForm()
{
    if (m_contentIn->toPlainText().isEmpty())
    {
        m_ok->setDisabled(true);
    }
    else
    {
        m_ok->setDisabled(false);
    }
}

void
ClaimDialog::accept()
{
    QString stripped = m_contentIn->toPlainText().trimmed();

    emit this->claimCreated(stripped);
    done(QDialog::Accepted);
}
