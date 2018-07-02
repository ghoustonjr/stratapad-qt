#include <QDebug>

#include "sp_parser.h"
#include "sp_util.h"

StratapadParser::StratapadParser(Tokenizer* t, Writer* s)
{
    m_tokenizer  = t;
    m_serializer = s;
}

// ==============
// GENERIC PARSER
// ==============

void
StratapadParser::parseBold(TokenData* data)
{
    TokenData* next;
    m_serializer->startElement(data);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != BOLD_CLOSE)
        {
            switch (next->tag)
            {
                case TEXT:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case CITATION:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case ITALIC:
                {
                    parseItalic(next);
                } break;

                case UNDERLINE:
                {
                    parseUnderline(next);
                } break;

                default:
                {
                    InvalidCodePath;
                }
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseItalic(TokenData* data)
{
    TokenData* next;
    m_serializer->startElement(data);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != ITALIC_CLOSE)
        {
            switch (next->tag)
            {
                case TEXT:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case CITATION:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case BOLD:
                {
                    parseBold(next);
                } break;

                case UNDERLINE:
                {
                    parseUnderline(next);
                } break;

                default:
                {
                    InvalidCodePath;
                }
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseUnderline(TokenData* data)
{
    TokenData* next;
    m_serializer->startElement(data);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != UNDERLINE_CLOSE)
        {
            switch (next->tag)
            {
                case TEXT:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case CITATION:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case BOLD:
                {
                    parseBold(next);
                } break;

                case ITALIC:
                {
                    parseItalic(next);
                } break;

                default:
                {
                    InvalidCodePath;
                }
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseParagraph(TokenData* current)
{
    TokenData* next;
    m_serializer->startElement(current);
    // TODO: Write block-level formatting info for this paragraph
    {
        next = m_tokenizer->nextToken();
        while (next->tag != PARAGRAPH_CLOSE)
        {
            switch (next->tag)
            {
                case TEXT:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case CITATION:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case BOLD:
                {
                    parseBold(next);
                } break;

                case ITALIC:
                {
                    parseItalic(next);
                } break;

                case UNDERLINE:
                {
                    parseUnderline(next);
                } break;

                default:
                {
                    InvalidCodePath;
                } break;
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseListItem(TokenData* current)
{
    TokenData* next;
    m_serializer->startElement(current);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != LIST_ITEM_CLOSE)
        {
            switch (next->tag)
            {
                case TEXT:
                {
                    m_serializer->startElement(next);
                    next->tag = static_cast<Token>(next->tag + 1);
                    m_serializer->endElement(next);
                } break;

                case BOLD:
                {
                    parseBold(next);
                } break;

                case ITALIC:
                {
                    parseItalic(next);
                } break;

                case UNDERLINE:
                {
                    parseUnderline(next);
                } break;

                default:
                {
                    InvalidCodePath;
                } break;
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseList(TokenData* current)
{
    TokenData* next;
    m_serializer->startElement(current);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != LIST_CLOSE)
        {
            switch (next->tag)
            {
                case LIST_ITEM:
                {
                    parseListItem(next);
                } break;

                default:
                {
                    InvalidCodePath;
                } break;
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseContents(TokenData* current)
{
    TokenData* next;
    m_serializer->startElement(current);
    {
        next = m_tokenizer->nextToken();
        while (next->tag != CONTENTS_CLOSE)
        {
            switch (next->tag)
            {
                case PARAGRAPH:
                {
                    parseParagraph(next);
                } break;

                case LIST:
                {
                    parseList(next);
                } break;

                default:
                {
                    InvalidCodePath;
                } break;
            }

            next = m_tokenizer->nextToken();
        }
    }
    m_serializer->endElement(next);
}

void
StratapadParser::parseDocument()
{
    m_tokenizer->tokenize();

    if (m_tokenizer->nextToken()->tag == DOCUMENT)
    {
        m_serializer->begin();

        TokenData* td = m_tokenizer->nextToken();
        if (td->tag == CONTENTS)
        {
            parseContents(td);

            if (m_tokenizer->nextToken()->tag == DOCUMENT_CLOSE)
            {
                m_serializer->end();
            }
        }
    }
}
