#include <QtWidgets>

#include "sp_tag_dialog.h"

sp_tag_dialog::sp_tag_dialog(QWidget *Parent) :
    QDialog(Parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setFixedSize(200, 75);
    setWindowTitle("Add Tag");
    QVBoxLayout* MainLayout = new QVBoxLayout(this);
    QFormLayout* Form = new QFormLayout;

    TagEditor  = new QLineEdit();

    Form->addRow("Tag", TagEditor);

    OkButton = new QPushButton("Create Tag");
    OkButton->setDisabled(true);
    QPushButton* CancelButton = new QPushButton("Cancel");

    QDialogButtonBox* Buttons = new QDialogButtonBox();
    Buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
    Buttons->addButton(CancelButton, QDialogButtonBox::RejectRole);

    MainLayout->addLayout(Form);
    MainLayout->addWidget(Buttons);

    setLayout(MainLayout);

    connect(TagEditor, &QLineEdit::textChanged, this, &sp_tag_dialog::ValidateForm);
    connect(OkButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void
sp_tag_dialog::ValidateForm()
{
    if(TagEditor->text().isEmpty())
    {
        OkButton->setDisabled(true);
    }
    else
    {
        OkButton->setDisabled(false);
    }
}

void
sp_tag_dialog::accept()
{
    QString Tag = TagEditor->text().trimmed();

    emit TagCreated(Tag);
    done(QDialog::Accepted);
}
