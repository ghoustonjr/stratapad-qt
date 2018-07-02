#ifndef READER_H
#define READER_H

#include <QStack>
#include <QQueue>
#include <QTextDocument>
#include <QXmlStreamReader>

#include "sp_parser_types.h"

struct FormatState {
    QStack<Token> stack;
    bool          wasBold;
    bool          wasItalic;
    bool          wasUnderline;
};

class Tokenizer {
public:
    virtual void tokenize()         = 0;
    virtual TokenData * nextToken() = 0;
};

class ReaderMem : public Tokenizer
{
public:
    ReaderMem(QTextDocument* doc);
    ~ReaderMem();

    void        tokenize() override;
    TokenData * nextToken() override;

    TokenData * addToken(Token tag);
    void        openToken(Token t, FormatState* fs);
    void        closeToken(Token t, FormatState* fs);
    void        closeOutstandingInlineTags(FormatState* fs);

    QQueue<TokenData *> m_tokenQueue;
    QTextDocument*      m_doc;
};

class ReaderXML : public Tokenizer {
public:
    ReaderXML(const QString& in);
    ~ReaderXML();

    void        tokenize() override;
    TokenData * nextToken() override;

    QList<TokenData *> m_tokenData;
    QXmlStreamReader*  m_reader;
};

#endif // READER_H
