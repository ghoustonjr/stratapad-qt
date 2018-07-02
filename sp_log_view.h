#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QTextBrowser>


class LogView : public QWidget
{
    Q_OBJECT
public:
    explicit
    LogView(QWidget* parent = nullptr);

    void addLogEntry(QString currentStateSHA, int timestamp, QString bodyAsHtml);

    QTextBrowser* m_log;

signals:

public slots:
};

#endif // LOGVIEW_H
