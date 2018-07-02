#ifndef WRITER_H
#define WRITER_H

#include <QString>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocument>
#include <QXmlStreamWriter>
#include <QTextFrameFormat>

#include "sp_parser_types.h"

class Writer
{
public:
    virtual void begin() = 0;
    virtual void startElement(TokenData* data) = 0;
    virtual void endElement(TokenData* data)   = 0;
    virtual void end() = 0;
};


class WriterXML : public Writer {
public:
    WriterXML();
    ~WriterXML();

    void begin() override;
    void startElement(TokenData* data) override;
    void endElement(TokenData* data)   override;
    void end() override;

    void writeBlockAttributes(TokenData* data);

    QXmlStreamWriter*    m_writer;
    QString*             m_doc;
    QMap<Token, QString> m_tags;
};

class WriterMem : public Writer {
public:
    WriterMem(QTextDocument* doc);

    void begin() override;
    void startElement(TokenData* data) override;
    void endElement(TokenData* data)   override;
    void end() override;

    int m_listStart;

    bool m_firstP;
    bool m_firstItem;
    bool m_trailingBlock;

    QTextCursor      m_c;
    QTextDocument*   m_doc;
    QTextFrameFormat m_rootFrameFormat;
    QTextCharFormat  m_originalCharFormat;
    QTextBlockFormat m_originalBlockFormat;

    QTextListFormat::Style m_listStyle;
};


class WriterTest : public Writer {
public:
    void begin() override;
    void end() override;

    void startElement(TokenData* data) override;
    void endElement(TokenData* data)   override;
};

#endif // SERIALIZER_H
