#ifndef CLAIMDIALOG_H
#define CLAIMDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "sp_db_manager.h"

class ClaimDialog : public QDialog
{
    Q_OBJECT

public:
    ClaimDialog(QWidget* parent = 0);

    spdb_manager*   m_repoManager;
    QComboBox*      m_topicIn;
    QPlainTextEdit* m_contentIn;
    QLineEdit*      m_titleIn;
    QPushButton*    m_ok;
    QLabel*         m_newTopicLabel;
    QLineEdit*      m_newTopicIn;
    QWidget*        m_topicSelect;
    QWidget*        m_addTopic;

public slots:
    void validateForm();
    void accept();

signals:
    void claimCreated(const QString& contents);
};

#endif // CLAIMDIALOG_H
