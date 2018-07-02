#include <QDebug>

#include "sp_writer.h"
#include "sp_textedit.h"

// ==============
// XML Serializer
// ==============

WriterXML::WriterXML()
{
    m_doc    = new QString;
    m_writer = new QXmlStreamWriter(m_doc);
    m_tags.insert(PARAGRAPH, PARAGRAPH_TAG);
    m_tags.insert(BOLD, BOLD_TAG);
    m_tags.insert(ITALIC, ITALIC_TAG);
    m_tags.insert(UNDERLINE, UNDERLINE_TAG);
    m_tags.insert(CITATION, CITATION_TAG);
    m_tags.insert(LIST, LIST_TAG);
    m_tags.insert(CONTENTS, CONTENTS_TAG);
    m_tags.insert(LIST_ITEM, LIST_ITEM_TAG);
}

WriterXML::~WriterXML()
{
    delete m_doc;
    delete m_writer;
}

void
WriterXML::writeBlockAttributes(TokenData* data)
{
    if (data->attributes & SET_ALIGN_LEFT)
    {
        m_writer->writeAttribute(ALIGN_ATTR_KEY, ALIGN_LEFT_ATTR);
    }
    else if (data->attributes & SET_ALIGN_CENTER)
    {
        m_writer->writeAttribute(ALIGN_ATTR_KEY, ALIGN_CENTER_ATTR);
    }
    else if (data->attributes & SET_ALIGN_RIGHT)
    {
        m_writer->writeAttribute(ALIGN_ATTR_KEY, ALIGN_RIGHT_ATTR);
    }
    else if (data->attributes & SET_ALIGN_JUSTIFY)
    {
        m_writer->writeAttribute(ALIGN_ATTR_KEY, ALIGN_JUSTIFY_ATTR);
    }
    else if (data->attributes & SET_DECIMAL_LIST)
    {
        m_writer->writeAttribute(LIST_TYPE_ATTR_KEY, LIST_TYPE_DECIMAL_ATTR);
    }
    else if (data->attributes & SET_DISC_LIST)
    {
        m_writer->writeAttribute(LIST_TYPE_ATTR_KEY, LIST_TYPE_DISC_ATTR);
    }
}

void
WriterXML::begin()
{
    m_writer->writeStartDocument();
}

void
WriterXML::startElement(TokenData* data)
{
    switch (data->tag)
    {
        case TEXT:
        {
            m_writer->writeCharacters(data->content);
        } break;

        case LIST:
        case PARAGRAPH:
        {
            m_writer->writeStartElement(m_tags.value(data->tag));
            writeBlockAttributes(data);
        } break;

        default:
        {
            m_writer->writeStartElement(m_tags.value(data->tag));
        } break;
    }
}

void
WriterXML::endElement(TokenData* data)
{
    switch (data->tag)
    {
        case TEXT_CLOSE:
        {
            return;
        } break;

        default:
        {
            m_writer->writeEndElement();
        } break;
    }
}

void
WriterXML::end()
{
    m_writer->writeEndDocument();
}
