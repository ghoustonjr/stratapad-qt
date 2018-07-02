#include <QTextBlock>
#include <QTextList>
#include <QFontInfo>
#include <QDebug>

#include "sp_reader.h"
#include "sp_textedit.h"
#include "sp_util.h"

ReaderMem::ReaderMem(QTextDocument* doc)
{
    m_doc = doc;
}

ReaderMem::~ReaderMem()
{
    qDeleteAll(m_tokenQueue);
}

void
ReaderMem::openToken(Token t, FormatState* fs)
{
    addToken(t);
    fs->stack.push(t);

    switch (t)
    {
        case BOLD:
        {
            fs->wasBold = true;
        } break;

        case ITALIC:
        {
            fs->wasItalic = true;
        } break;

        case UNDERLINE:
        {
            fs->wasUnderline = true;
        } break;
    }
}

void
ReaderMem::closeToken(Token t, FormatState* fs)
{
    Token top;
    do
    {
        top = fs->stack.pop();
        switch (top)
        {
            case BOLD:
            {
                fs->wasBold = false;
            } break;

            case ITALIC:
            {
                fs->wasItalic = false;
            } break;

            case UNDERLINE:
            {
                fs->wasUnderline = false;
            } break;
        }
        addToken(static_cast<Token>(top + 1));
    }
    while (top != t);
}

void
ReaderMem::closeOutstandingInlineTags(FormatState* fs)
{
    while (!fs->stack.isEmpty())
    {
        Token top = fs->stack.pop();
        addToken(static_cast<Token>(top + 1));
    }
}

TokenData *
ReaderMem::addToken(Token tag)
{
    TokenData* result = new TokenData;
    *result     = { };
    result->tag = tag;
    m_tokenQueue.enqueue(result);

    return result;
}

void
recordListAttributes(QTextBlock b, TokenData* data)
{
    if (b.textList())
    {
        QTextListFormat::Style style = b.textList()->format().style();
        switch (style)
        {
            case QTextListFormat::ListDisc:
            {
                data->attributes |= SET_DISC_LIST;
            } break;

            case QTextListFormat::ListDecimal:
            {
                data->attributes |= SET_DECIMAL_LIST;
            } break;
        }
    }
}

void
recordBlockAttributes(QTextBlock b, TokenData* data)
{
    Qt::Alignment a = b.blockFormat().alignment();
    switch (a)
    {
        case Qt::AlignLeft:
        {
            data->attributes |= SET_ALIGN_LEFT;
        } break;

        case Qt::AlignCenter:
        {
            data->attributes |= SET_ALIGN_CENTER;
        } break;

        case Qt::AlignRight:
        {
            data->attributes |= SET_ALIGN_RIGHT;
        } break;

        case Qt::AlignJustify:
        {
            data->attributes |= SET_ALIGN_JUSTIFY;
        } break;
    }
}

void
ReaderMem::tokenize()
{
    this->m_tokenQueue.clear();

    addToken(DOCUMENT);
    addToken(CONTENTS);
    {
        bool openedListItem = false;
        bool areInList      = false;
        bool wereInList     = false;
        QTextList* lastList = NULL;

        for (QTextBlock b = m_doc->begin(); b != m_doc->end(); b = b.next())
        {
            openedListItem = false;

            if (b.isValid())
            {
                // Figuring out whether the block we're in is a list or a paragraph, and recording the relevant attributes for each block type.
                TokenData* blockData = NULL;
                areInList = (b.textList() != NULL);

                switch (runCategory(wereInList, areInList))
                {
                    case RUN_NONE:
                    {
                        blockData = addToken(PARAGRAPH);
                    } break;

                    case RUN_START:
                    {
                        openedListItem = true;

                        TokenData* listData = addToken(LIST);
                        recordListAttributes(b, listData);

                        blockData = addToken(LIST_ITEM);
                    } break;

                    case RUN_INSIDE:
                    {
                        if (b.textList() == lastList)
                        {
                            openedListItem = true;
                            blockData      = addToken(LIST_ITEM);
                        }
                        else
                        {
                            openedListItem = true;
                            addToken(LIST_CLOSE);

                            TokenData* listData = addToken(LIST);
                            recordListAttributes(b, listData);

                            blockData = addToken(LIST_ITEM);
                        }
                    } break;

                    case RUN_END:
                    {
                        addToken(LIST_CLOSE);
                        blockData = addToken(PARAGRAPH);
                    } break;
                }

                recordBlockAttributes(b, blockData);

                // Figure out what to do at the fragment level
                FormatState fs = { };
                QTextBlock::iterator it;
                bool firstP = true;
                for (it = b.begin(); !it.atEnd(); ++it)
                {
                    QTextFragment frag = it.fragment();
                    if (frag.isValid())
                    {
                        QTextCharFormat fmt = frag.charFormat();
                        QFontInfo font      = fmt.font();

                        // Handle closers first, provided we're not in the current fragment.
                        if (!firstP)
                        {
                            if (runCategory(fs.wasBold, font.bold()) == RUN_END)
                            {
                                closeToken(BOLD, &fs);
                            }

                            if (runCategory(fs.wasItalic, font.italic()) == RUN_END)

                            {
                                closeToken(ITALIC, &fs);
                            }

                            if (runCategory(fs.wasUnderline, font.underline()) == RUN_END)
                            {
                                closeToken(UNDERLINE, &fs);
                            }
                        }
                        else
                        {
                            firstP = false;
                        }

                        // Handle openers second
                        if (runCategory(fs.wasBold, font.bold()) == RUN_START)
                        {
                            openToken(BOLD, &fs);
                        }

                        if (runCategory(fs.wasItalic, font.italic()) == RUN_START)
                        {
                            openToken(ITALIC, &fs);
                        }

                        if (runCategory(fs.wasUnderline, font.underline()) == RUN_START)
                        {
                            openToken(UNDERLINE, &fs);
                        }


                        if (fmt.objectType() == sp_textedit::CiteObjectType)
                        {
                            QString claim = fmt.property(sp_textedit::CitationData).toString();
                            TokenData* c  = addToken(CITATION);
                            c->content = claim;

                            // addToken(CITATION_CLOSE);
                        }
                        else
                        {
                            TokenData* t = addToken(TEXT);
                            t->content = frag.text();
                        }
                    }
                }

                closeOutstandingInlineTags(&fs);

                if (openedListItem)
                {
                    addToken(LIST_ITEM_CLOSE);
                }
                else
                {
                    addToken(PARAGRAPH_CLOSE);
                }

                wereInList = areInList;
                lastList   = b.textList();
            }
        }

        if (openedListItem)
        {
            addToken(LIST_CLOSE);
        }
    }
    addToken(CONTENTS_CLOSE);
    addToken(DOCUMENT_CLOSE);
}

TokenData *
ReaderMem::nextToken()
{
    return this->m_tokenQueue.dequeue();
}
