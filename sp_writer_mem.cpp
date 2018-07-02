#include <QDebug>

#include "sp_writer.h"
#include "sp_textedit.h"

WriterMem::WriterMem(QTextDocument* doc)
{
    m_doc = doc;
    m_c   = QTextCursor(m_doc);
}

void
WriterMem::begin()
{
    m_trailingBlock   = false;
    m_firstP          = true;
    m_firstItem       = true;
    m_listStart       = 0;
    m_listStyle       = QTextListFormat::ListDisc;
    m_rootFrameFormat = m_c.currentFrame()->frameFormat();
    m_rootFrameFormat.setLeftMargin(15);
    m_rootFrameFormat.setRightMargin(15);
    m_c.currentFrame()->setFrameFormat(m_rootFrameFormat);
    m_originalBlockFormat = m_c.blockFormat();
    m_originalBlockFormat.setTopMargin(14.0f);
    m_originalBlockFormat.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
    m_originalCharFormat = m_c.blockCharFormat();
}

void
WriterMem::startElement(TokenData* data)
{
    switch (data->tag)
    {
        case TEXT:
        {
            m_c.insertText(data->content);
        } break;

        case LIST:
        {
            if (data->attributes & SET_DECIMAL_LIST)
            {
                m_listStyle = QTextListFormat::ListDecimal;
            }
            else if (data->attributes & SET_DISC_LIST)
            {
                m_listStyle = QTextListFormat::ListDisc;
            }
        } break;

        case LIST_ITEM:
        {
            m_c.setBlockFormat(m_originalBlockFormat);
            m_c.setBlockCharFormat(m_originalCharFormat);
            m_c.insertBlock();
            if (m_firstItem)
            {
                m_listStart = m_c.position();
                m_firstItem = false;
            }
        } break;

        case PARAGRAPH:
        {
            Qt::Alignment align;

            if (data->attributes & SET_ALIGN_LEFT)
            {
                align = Qt::AlignLeft;
            }
            else if (data->attributes & SET_ALIGN_CENTER)
            {
                align = Qt::AlignCenter;
            }
            else if (data->attributes & SET_ALIGN_RIGHT)
            {
                align = Qt::AlignRight;
            }
            else
            {
                align = Qt::AlignJustify;
            }

            if (!m_firstP)
            {
                m_c.insertBlock();
            }

            QTextBlockFormat fmt = m_originalBlockFormat;
            fmt.setAlignment(align);

            m_c.setBlockCharFormat(m_originalCharFormat);
            m_c.setBlockFormat(fmt);
            m_firstP = false;
        } break;

        case BOLD:
        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Bold);
            m_c.mergeCharFormat(fmt);
        } break;

        case UNDERLINE:
        {
            QTextCharFormat fmt;
            fmt.setFontUnderline(true);
            m_c.mergeCharFormat(fmt);
        } break;

        case ITALIC:
        {
            QTextCharFormat fmt;
            fmt.setFontItalic(true);
            m_c.mergeCharFormat(fmt);
        } break;

        case CITATION:
        {
            QString claimSHA = data->content;

            QTextCharFormat fmt;
            QTextCharFormat restore = m_c.charFormat();

            fmt.setObjectType(sp_textedit::CiteObjectType);
            fmt.setProperty(sp_textedit::CitationData, claimSHA);
            fmt.merge(m_c.charFormat());

            m_c.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
            m_c.setCharFormat(restore);
        } break;

        default:
        { } break;
    }
}

void
WriterMem::endElement(TokenData* data)
{
    switch (data->tag)
    {
        case BOLD_CLOSE:
        {
            QTextCharFormat fmt;
            fmt.setFontWeight(QFont::Normal);
            m_c.mergeCharFormat(fmt);
        } break;

        case UNDERLINE_CLOSE:
        {
            QTextCharFormat fmt;
            fmt.setFontUnderline(false);
            m_c.mergeCharFormat(fmt);
        } break;

        case ITALIC_CLOSE:
        {
            QTextCharFormat fmt;
            fmt.setFontItalic(false);
            m_c.mergeCharFormat(fmt);
        } break;

        case LIST_CLOSE:
        {
            QTextCursor c2(m_c);
            c2.setPosition(m_listStart, QTextCursor::KeepAnchor);
            c2.createList(m_listStyle);
            m_listStart = 0;
            m_firstItem = true;
            m_listStyle = QTextListFormat::ListDisc;
        } break;

        case PARAGRAPH_CLOSE:
        {
            // m_c.insertBlock();
            m_trailingBlock = true;
        } break;


        case CONTENTS_CLOSE:
        {
            // I have to snip off the final block from the doc (it's guaranteed to be empty). I do this
            // because for every paragraph I close (see PARAGRAPH_CLOSE above), I call insertBlock to move the cursor
            // into a new block that will get filled out by subsequent tokens. Since there will be no more tokens from
            // here one, I can stop.
            // m_c.select(QTextCursor::BlockUnderCursor);
            // m_c.removeSelectedText();
            // m_c.deletePreviousChar();
        } break;

        default:
        { } break;
    }
}

void
WriterMem::end()
{
    // DO NOTHING
}

// ====================
// Testing stuff below.
// ====================

void
WriterTest::begin()
{
    qDebug() << "Starting the document.";
}

void
WriterTest::startElement(TokenData* data)
{
    qDebug() << "Opening element: " << tokenString(data->tag);
    if (data->tag == TEXT)
    {
        qDebug() << data->content;
    }
}

void
WriterTest::endElement(TokenData* data)
{
    qDebug() << "Closing element: " << tokenString(data->tag);
}

void
WriterTest::end()
{
    qDebug() << "Ending the document";
}
