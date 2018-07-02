#ifndef SP_TAG_DIALOG_H
#define SP_TAG_DIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "sp_types.h"

class sp_tag_dialog : public QDialog
{
    Q_OBJECT

public:
    sp_tag_dialog(QWidget *Parent=0);

    QLineEdit *TagEditor;
    QPushButton *OkButton;

public slots:
    void ValidateForm();
    void accept();

signals:
    void TagCreated(const QString &Tag);
};

#endif // SP_TAG_DIALOG_H
