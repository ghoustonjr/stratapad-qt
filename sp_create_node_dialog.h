#ifndef CREATE_NODE_DIALOG_H
#define CREATE_NODE_DIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

#include "sp_types.h"
#include "sp_db_manager.h"

class CreateNodeDialog : public QDialog
{
    Q_OBJECT

public:
    CreateNodeDialog(QWidget* parent = 0);

    spdb_manager   *RepoManager;
    QComboBox      *TypeSelect;
    QLineEdit      *TitleEditor;
    QPushButton    *OkButton;

 public slots:
    void ValidateForm();
    void accept();

signals:
    void NodeCreated(basic_node Title);

};

#endif // CREATE_NODE_DIALOG_H
