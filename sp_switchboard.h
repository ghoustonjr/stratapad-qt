#ifndef SWITCHBOARD_H
#define SWITCHBOARD_H

#include <QObject>
#include <QAction>

#include "sp_textedit.h"

class Switchboard : public QObject
{
    Q_OBJECT
public:
    static Switchboard * globalSwitchBoard();

    void switchEditor(sp_textedit* editor);

private:
    Switchboard();

    QAction* m_textBold;
    QAction* m_textItalic;
    QAction* m_textUnderline;

    sp_textedit* m_editor;

signals:

public slots:
};

#endif // SWITCHBOARD_H
