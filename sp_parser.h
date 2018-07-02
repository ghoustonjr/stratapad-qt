#ifndef STRATAPADPARSER_H
#define STRATAPADPARSER_H

#include <QObject>
#include <QWidget>
#include <QTextDocument>
#include <QXmlStreamWriter>
#include <QStack>
#include <QQueue>
#include <stdint.h>
#include "sp_parser_types.h"
#include "sp_reader.h"
#include "sp_writer.h"

class StratapadParser
{
public:
    StratapadParser(Tokenizer* t, Writer* s);

    void        parseDocument();
    void        parseList(TokenData* td);
    void        parseBold(TokenData* td);
    void        parseItalic(TokenData* td);
    void        parseListItem(TokenData* td);
    void        parseContents(TokenData* td);
    void        parseUnderline(TokenData* td);
    void        parseParagraph(TokenData* td);

    Tokenizer*  m_tokenizer;
    Writer* m_serializer;
};

#endif // STRATAPADPARSER_H
