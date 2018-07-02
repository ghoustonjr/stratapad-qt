#include <QDebug>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QMenu>
#include <QDateTime>
#include <QTextList>

#include "sp_textedit.h"
#include "sp_link_text_object.h"
#include "sp_util.h"

sp_textedit::sp_textedit(QWidget* parent) : QTextEdit(parent)
{
    // =============
    // SETUP ACTIONS
    // =============

    m_makeClaim = new QAction("Make claim from selection...", this);
    m_makeClaim->setEnabled(false);

    m_insertCitation = new QAction("Link to claim...", this);

    connect(m_makeClaim, &QAction::triggered, this, &sp_textedit::makeClaim);
    connect(this, &QTextEdit::copyAvailable, this, &sp_textedit::enableActions);
    connect(m_insertCitation, &QAction::triggered, this, &sp_textedit::requestAddCitation);

    // ===========================
    // SETUP CUSTOM OBJECT SUPPORT
    // ===========================

    setMouseTracking(true);

    QObject* linkInterface = new LinkTextObject;
    linkInterface->setParent(this);
    document()->documentLayout()->registerHandler(sp_textedit::CiteObjectType, linkInterface);
}

// =========================
// TEXT FORMATTING FUNCTIONS
// =========================

void
sp_textedit::textBold(bool toggle)
{
    QTextCharFormat fmt;

    fmt.setFontWeight(toggle ? QFont::Bold : QFont::Normal);
    mergeCharFormat(fmt);
}

void
sp_textedit::textUnderline(bool toggle)
{
    QTextCharFormat fmt;

    fmt.setFontUnderline(toggle);
    mergeCharFormat(fmt);
}

void
sp_textedit::textItalic(bool toggle)
{
    QTextCharFormat fmt;

    fmt.setFontItalic(toggle);
    mergeCharFormat(fmt);
}

void
sp_textedit::mergeCharFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    cursor.mergeCharFormat(format);
    setTextCursor(cursor);
}

// =========================
// CUSTOM OBJECT INTERACTION
// =========================

void
sp_textedit::mouseMoveEvent(QMouseEvent* event)
{
    QTextEdit::mouseMoveEvent(event);

    QTextFormat f = document()->documentLayout()->formatAt(event->pos());

    if (f.objectType() == CiteObjectType)
    {
        viewport()->setCursor(Qt::PointingHandCursor);
    }
    else
    {
        viewport()->setCursor(Qt::IBeamCursor);
    }
}

void
sp_textedit::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::InsertParagraphSeparator) || (event->key() == Qt::Key_Backspace))
    {
        QTextCursor c   = textCursor();
        QTextList* list = c.currentList();
        if (list)
        {
            if (c.block().text().trimmed().isEmpty())
            {
                c.beginEditBlock();
                {
                    removeList();
                }
                c.endEditBlock();

                return;
            }
        }
    }

    QTextEdit::keyPressEvent(event);
}

// ============
// CONTEXT MENU
// ============

void
sp_textedit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu();

    menu->addSeparator();
    menu->addAction(m_insertCitation);
    menu->addAction(m_makeClaim);

    menu->exec(event->globalPos());
    delete menu;
}

// ===============
// ACTION HANDLERS
// ===============
void
sp_textedit::insertCitation(const QString& claimSHA)
{
    QTextCharFormat fmt;
    QTextCursor c = textCursor();

    c.insertText(QString(QChar(0x2060)));

    fmt.setObjectType(sp_textedit::CiteObjectType);
    fmt.setProperty(sp_textedit::CitationData, claimSHA);
    fmt.merge(c.charFormat());

    c.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
}

void
sp_textedit::makeClaim()
{
    QTextCursor c = textCursor();

    if (c.hasSelection())
    {
        emit requestMakeClaim(c.selectedText());
    }
}

void
sp_textedit::enableActions(bool toggle)
{
    m_makeClaim->setEnabled(toggle);
}

// ===========================
//  BLOCK FORMATTING FUNCTIONS
// ===========================

void
sp_textedit::handleUnorderedList()
{
    handleList(QTextListFormat::ListDisc);
}

void
sp_textedit::handleOrderedList()
{
    handleList(QTextListFormat::ListDecimal);
}

void
sp_textedit::handleList(QTextListFormat::Style style)
{
    QTextCursor c      = textCursor();
    QTextBlock current = c.block();

    QTextList* listCurrent = current.textList();
    QTextList* listBefore  = 0;
    QTextList* listAfter   = 0;
    bool hasSelection      = c.hasSelection();
    bool plainBlocks       = false;
    bool listBlocks        = false;
    bool listsMixed        = false;

    if (hasSelection)
    {
        int start = c.selectionStart();
        int end   = c.selectionEnd();
        QTextCursor inspectNeighbors(c);

        inspectNeighbors.setPosition(start);
        QTextBlock startBlock = inspectNeighbors.block();

        inspectNeighbors.setPosition(end);
        QTextBlock endBlock = inspectNeighbors.block();

        listBefore = startBlock.previous().textList();
        listAfter  = endBlock.next().textList();

        QTextList* lastList = startBlock.textList();
        while (startBlock != endBlock.next())
        {
            QTextList* list = startBlock.textList();
            if (list)
            {
                listBlocks = true;
                if (list != lastList)
                {
                    listsMixed = true;
                }
            }
            else
            {
                plainBlocks = true;
            }

            lastList   = list;
            startBlock = startBlock.next();
        }
    }
    else
    {
        listBefore = current.previous().textList();
        listAfter  = current.next().textList();
    }

    list_decision d;
    d.b            = listBefore;
    d.c            = listCurrent;
    d.a            = listAfter;
    d.c_style      = style;
    d.hasSelection = hasSelection;
    d.listBlocks   = listBlocks;
    d.plainBlocks  = plainBlocks;
    d.listsMixed   = listsMixed;

    list_case action = hasSelection ? caseManyBlocks(d) : caseOneBlock(d);
    switch (action)
    {
        case CREATE_LIST:
        {
            qDebug() << "Creating list";
            c.beginEditBlock();
            {
                createList(style);
            }
            c.endEditBlock();
        } break;

        case REMOVE_LIST:
        {
            qDebug() << "Removing list";
            c.beginEditBlock();
            {
                removeList();
            }
            c.endEditBlock();
        } break;

        case JOIN_BC:
        {
            qDebug() << "Append current to B";
            Q_ASSERT(listBefore);
            c.beginEditBlock();
            {
                listCurrent = createList(style);
                Q_ASSERT(listCurrent);

                appendToList(listBefore, listCurrent);
            }
            c.endEditBlock();
        } break;

        case JOIN_AC:
        {
            qDebug() << "Prepend current to A";
            Q_ASSERT(listAfter);
            c.beginEditBlock();
            {
                listCurrent = createList(style);
                Q_ASSERT(listCurrent);

                appendToList(listCurrent, listAfter);
            }
            c.endEditBlock();
        } break;

        case JOIN_BCA:
        {
            qDebug() << "Create C, then join B C A";
            Q_ASSERT(listBefore);
            Q_ASSERT(listAfter);
            c.beginEditBlock();
            {
                listCurrent = createList(style);
                Q_ASSERT(listCurrent);

                appendToList(listBefore, listCurrent);
                appendToList(listBefore, listAfter);
            }
            c.endEditBlock();
        } break;

        case SPLIT_LIST:
        {
            qDebug() << "Split list.";
            Q_ASSERT(listBefore);
            Q_ASSERT(listCurrent);
            Q_ASSERT(listAfter);
            Q_ASSERT((listBefore == listAfter) && (listAfter == listCurrent));
            c.beginEditBlock();
            {
                QTextBlock splitEnd = current;
                if (hasSelection)
                {
                    QTextCursor c2(c);
                    c2.setPosition(c.selectionEnd());
                    splitEnd = c2.block();
                }

                splitList(listCurrent, current, splitEnd);
            }
            c.endEditBlock();
        } break;

        InvalidDefaultCase;
    }
}

QTextList *
sp_textedit::createList(QTextListFormat::Style style)
{
    QTextCursor c = textCursor();

    if (c.block() == document()->begin())
    {
        QTextCursor c2(c);
        c2.setPosition(0);
        c2.insertBlock();
    }

    c.createList(style);

    return c.currentList();
}

// CR-TODO: This seems to work well, but these QTextLists are a little bit finicky. So revisit it and do a careful step through.
void
sp_textedit::appendToList(QTextList* list, QTextList* toAppend)
{
    QList<QTextBlock> blockList;
    int appendSize = toAppend->count();
    for (int i = 0; i < appendSize; i++)
    {
        blockList.append(toAppend->item(i));
    }

    for (QTextBlock b : blockList)
    {
        list->add(b);
    }
}

void
sp_textedit::removeList()
{
    QTextCursor c = textCursor();
    c.beginEditBlock();
    {
        QTextCursor c2(c);
        if (c.hasSelection())
        {
            c2.setPosition(c.selectionStart());
            QTextBlock startBlock = c2.block();

            c2.setPosition(c.selectionEnd());
            QTextBlock endBlock = c2.block();

            while (startBlock != endBlock.next())
            {
                QTextList* list = startBlock.textList();
                if (list)
                {
                    list->remove(startBlock);
                    QTextBlockFormat fmt = startBlock.blockFormat();
                    fmt.setIndent(0);
                    c.mergeBlockFormat(fmt);
                }

                startBlock = startBlock.next();
            }
        }
        else
        {
            QTextList* list = c2.block().textList();
            if (list)
            {
                list->remove(c2.block());
                QTextBlockFormat fmt = c2.block().blockFormat();
                fmt.setIndent(0);
                c2.mergeBlockFormat(fmt);
            }
        }
    }
    c.endEditBlock();
}

void
sp_textedit::splitList(QTextList* list, QTextBlock splitStart, QTextBlock splitEnd)
{
    QTextCursor c = textCursor();
    QTextBlock parentListStart;
    QTextBlock parentListEnd;
    QTextBlock lastBeforeSplit = splitStart.previous();
    QTextListFormat::Style splitAtStyle = list->format().style();

    int count = list->count();
    parentListStart = list->item(0);
    parentListEnd   = list->item(count - 1);

    QTextBlock it  = parentListStart;
    QTextBlock end = parentListEnd.next();

    while (it != end)
    {
        list->remove(it);
        c.setPosition(it.position());
        QTextBlockFormat fmt = it.blockFormat();
        fmt.setIndent(0);
        c.mergeBlockFormat(fmt);
        it = it.next();
    }

    c.setPosition(splitStart.position());

    c.setPosition(parentListStart.position());
    c.setPosition(lastBeforeSplit.position(), QTextCursor::KeepAnchor);
    c.createList(splitAtStyle);

    c.setPosition(splitEnd.next().position());
    c.setPosition(parentListEnd.position(), QTextCursor::KeepAnchor);
    c.createList(splitAtStyle);
}

list_case
sp_textedit::caseOneBlock(list_decision d)
{
    if (d.c)
    {
        if (d.a && d.b)
        {
            if ((d.a == d.b) && (d.b == d.c))
            {
                return SPLIT_LIST;
            }
        }

        return REMOVE_LIST;
    }
    else
    {
        return caseNoListBlocks(d);
    }
}

list_case
sp_textedit::caseManyBlocks(list_decision d)
{
    if (d.plainBlocks)
    {
        if (d.listBlocks)
        {
            return REMOVE_LIST;
        }
        else
        {
            return caseNoListBlocks(d);
        }
    }
    else
    {
        if (d.listBlocks)
        {
            if (d.listsMixed)
            {
                return REMOVE_LIST;
            }
            else
            {
                if ((d.a && d.b) && (d.a == d.b))
                {
                    return SPLIT_LIST;
                }
                else
                {
                    return REMOVE_LIST;
                }
            }
        }
        else
        {
            return NOP;
        }
    }
}

list_case
sp_textedit::caseNoListBlocks(list_decision d)
{
    if (d.b)
    {
        QTextListFormat::Style b_style = d.b->format().style();
        if (d.a)
        {
            QTextListFormat::Style a_style = d.a->format().style();
            bool ab_same = (b_style == a_style);
            bool cb_same = (b_style == d.c_style);
            bool ca_same = (a_style == d.c_style);
            if (ab_same && ca_same)
            {
                return JOIN_BCA;
            }
            else if (cb_same)
            {
                return JOIN_BC;
            }
            else if (ca_same)
            {
                return JOIN_AC;
            }
            else
            {
                return CREATE_LIST;
            }
        }
        else
        {
            if (b_style == d.c_style)
            {
                return JOIN_BC;
            }
            else
            {
                return CREATE_LIST;
            }
        }
    }
    else
    {
        if (d.a)
        {
            QTextListFormat::Style a_style = d.a->format().style();
            if (d.c_style == a_style)
            {
                return JOIN_AC;
            }
            else
            {
                return CREATE_LIST;
            }
        }
        else
        {
            return CREATE_LIST;
        }
    }
}
