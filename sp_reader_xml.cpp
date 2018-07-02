#include "sp_reader.h"

static QMap<QString, Token> openers = {
    { PARAGRAPH_TAG, PARAGRAPH },
    { BOLD_TAG,      BOLD      },
    { ITALIC_TAG,    ITALIC    },
    { UNDERLINE_TAG, UNDERLINE },
    { CITATION_TAG,  CITATION  },
    { LIST_TAG,      LIST      },
    { LIST_ITEM_TAG, LIST_ITEM },
    { CONTENTS_TAG,  CONTENTS  }
};

static QMap<QString, Token> closers = {
    { PARAGRAPH_TAG, PARAGRAPH_CLOSE },
    { BOLD_TAG,      BOLD_CLOSE      },
    { ITALIC_TAG,    ITALIC_CLOSE    },
    { UNDERLINE_TAG, UNDERLINE_CLOSE },
    { CITATION_TAG,  CITATION_CLOSE  },
    { LIST_TAG,      LIST_CLOSE      },
    { LIST_ITEM_TAG, LIST_ITEM_CLOSE },
    { CONTENTS_TAG,  CONTENTS_CLOSE  }
};

ReaderXML::ReaderXML(const QString& in)
{
    m_reader = new QXmlStreamReader(in);
}

ReaderXML::~ReaderXML()
{
    qDeleteAll(m_tokenData);
    delete m_reader;
}

void
ReaderXML::tokenize(){ }

TokenData *
ReaderXML::nextToken()
{
    TokenData* result = new TokenData();
    QXmlStreamReader::TokenType type;

    if (!m_reader->atEnd())
    {
        type = m_reader->readNext();
        switch (type)
        {
            case QXmlStreamReader::StartDocument:
            {
                result->tag = DOCUMENT;
            } break;

            case QXmlStreamReader::StartElement:
            {
                QString name = m_reader->name().toString();

                if (name == PARAGRAPH_TAG)
                {
                    QXmlStreamAttributes attr = m_reader->attributes();
                    QString align = attr.value(ALIGN_ATTR_KEY).toString();

                    result->tag = PARAGRAPH;

                    if (align == ALIGN_LEFT_ATTR)
                    {
                        result->attributes |= SET_ALIGN_LEFT;
                    }
                    else if (align == ALIGN_CENTER_ATTR)
                    {
                        result->attributes |= SET_ALIGN_CENTER;
                    }
                    else if (align == ALIGN_RIGHT_ATTR)
                    {
                        result->attributes |= SET_ALIGN_RIGHT;
                    }
                    else
                    {
                        result->attributes |= SET_ALIGN_JUSTIFY;
                    }
                }
                else if (name == CITATION_TAG)
                {
                    QXmlStreamAttributes attr = m_reader->attributes();
                    QString claimSHA = attr.value(SHA_ATTR_KEY).toString();

                    result->tag     = CITATION;
                    result->content = claimSHA;
                }
                else if (name == LIST_TAG)
                {
                    result->tag = LIST;
                    QXmlStreamAttributes attr = m_reader->attributes();
                    QString listType = attr.value(LIST_TYPE_ATTR_KEY).toString();

                    if (listType == LIST_TYPE_DECIMAL_ATTR)
                    {
                        result->attributes |= SET_DECIMAL_LIST;
                    }
                    else
                    {
                        result->attributes |= SET_DISC_LIST;
                    }
                }
                else
                {
                    result->tag = openers.value(name);
                }
            } break;

            case QXmlStreamReader::EndElement:
            {
                QString name = m_reader->name().toString();

                if (name == CITATION_TAG)
                {
                    return nextToken();
                }

                result->tag = closers.value(name);
            } break;

            case QXmlStreamReader::Characters:
            {
                result->tag     = TEXT;
                result->content = m_reader->text().toString();
            } break;

            case QXmlStreamReader::EndDocument:
            {
                result->tag = DOCUMENT_CLOSE;
            } break;
        }
    }


    return result;
}
