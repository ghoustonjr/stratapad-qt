#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>

#include "sp_create_node_dialog.h"

CreateNodeDialog::CreateNodeDialog(QWidget *Parent) : QDialog(Parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle("Create New Node");
    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    QFormLayout* Form = new QFormLayout;

    TypeSelect = new QComboBox;
    TitleEditor  = new QLineEdit();

    Form->addRow("Node Type", TypeSelect);
    Form->addRow("Title", TitleEditor);

    OkButton = new QPushButton("Create Node");
    OkButton->setDisabled(true);
    QPushButton* CancelButton = new QPushButton("Cancel");

    QDialogButtonBox* Buttons = new QDialogButtonBox();
    Buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
    Buttons->addButton(CancelButton, QDialogButtonBox::RejectRole);

    MainLayout->addLayout(Form);
    MainLayout->addWidget(Buttons);

    setLayout(MainLayout);

    connect(TitleEditor, &QLineEdit::textChanged, this, &CreateNodeDialog::ValidateForm);
    connect(OkButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void
CreateNodeDialog::ValidateForm()
{
    if(TitleEditor->text().isEmpty())
    {
        OkButton->setDisabled(true);
    }
    else
    {
        OkButton->setDisabled(false);
    }
}

void
CreateNodeDialog::accept()
{
    basic_node Node = {};
    Node.Title = TitleEditor->text().trimmed();

    emit this->NodeCreated(Node);
    done(QDialog::Accepted);
}
