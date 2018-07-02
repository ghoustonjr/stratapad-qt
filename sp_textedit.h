#ifndef SP_TEXTEDIT_H
#define SP_TEXTEDIT_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QMouseEvent>
#include <QAction>
#include "sp_types.h"

struct list_decision {
    QTextList *b;
    QTextList *c;
    QTextList *a;
    QTextListFormat::Style c_style;

    bool hasSelection;
    bool plainBlocks;
    bool listBlocks;
    bool listsMixed;
};

enum list_case {
    // Cases that can occur with or without a selection
    NOP,
    CREATE_LIST,
    REMOVE_LIST,
    JOIN_AC,
    JOIN_BC,
    JOIN_BCA,
    SPLIT_LIST
};

class sp_textedit : public QTextEdit
{
    Q_OBJECT
public:
    enum { CiteObjectType = QTextFormat::UserObject + 1 };
    enum CiteProps { CitationData = QTextFormat::UserProperty + 1 };

    sp_textedit(QWidget* parent = 0);

    void mergeCharFormat(const QTextCharFormat &format);

    void        removeList();
    void        handleOrderedList();
    void        handleUnorderedList();
    void        handleList(QTextListFormat::Style style);
    void        appendToList(QTextList* list, QTextList* toAppend);
    void        splitList(QTextList* list, QTextBlock splitAt, QTextBlock splitEnd);

    QTextList * createList(QTextListFormat::Style style);

    // =======================
    // LIST DECISION FUNCTIONS
    // =======================
    list_case    caseOneBlock(list_decision data);
    list_case    caseNoListBlocks(list_decision data);
    list_case    caseManyBlocks(list_decision data);

    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    QAction* m_makeClaim;
    QAction* m_insertCitation;

public slots:
    // =========================
    // TEXT FORMATTING FUNCTIONS
    // =========================
    void textBold(bool toggle);
    void textItalic(bool toggle);
    void textUnderline(bool toggle);

    void enableActions(bool yes);

    void makeClaim();
    void insertCitation(const QString& claimSHA);

signals:
    void requestAddCitation();
    void requestMakeClaim(const QString& claim);
};

#endif
