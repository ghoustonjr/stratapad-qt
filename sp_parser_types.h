#ifndef PARSERTYPES_H
#define PARSERTYPES_H

// XML tag values
#define PARAGRAPH_TAG "p"
#define BOLD_TAG      "b"
#define ITALIC_TAG    "i"
#define UNDERLINE_TAG "u"
#define CITATION_TAG  "citation"
#define LIST_TAG      "list"
#define LIST_ITEM_TAG "item"
#define CONTENTS_TAG  "contents"

// XML attribute keys
#define SHA_ATTR_KEY       "sha"
#define ALIGN_ATTR_KEY     "align"
#define LIST_TYPE_ATTR_KEY "type"

// XML attribute values
#define ALIGN_LEFT_ATTR        "left"
#define ALIGN_CENTER_ATTR      "center"
#define ALIGN_RIGHT_ATTR       "right"
#define ALIGN_JUSTIFY_ATTR     "justify"
#define LIST_TYPE_DECIMAL_ATTR "decimal"
#define LIST_TYPE_DISC_ATTR    "disc"

// Mask values for TokenData "attributes" var
#define SET_ALIGN_LEFT      1u << 0
#define SET_ALIGN_CENTER    1u << 1
#define SET_ALIGN_RIGHT     1u << 2
#define SET_ALIGN_JUSTIFY   1u << 3
#define SET_EMPTY_PARAGRAPH 1u << 4
#define SET_DECIMAL_LIST    1u << 5
#define SET_DISC_LIST       1u << 6

enum Token {
    TEXT,
    TEXT_CLOSE,
    PARAGRAPH,
    PARAGRAPH_CLOSE,
    BOLD,
    BOLD_CLOSE,
    ITALIC,
    ITALIC_CLOSE,
    UNDERLINE,
    UNDERLINE_CLOSE,
    CITATION,
    CITATION_CLOSE,
    LIST,
    LIST_CLOSE,
    LIST_ITEM,
    LIST_ITEM_CLOSE,
    DOCUMENT,
    DOCUMENT_CLOSE,
    CONTENTS,
    CONTENTS_CLOSE,
};

static inline const char *
tokenString(enum Token t)
{
    static const char* strings[] = {
        "TEXT",
        "TEXT_CLOSE",
        "PARAGRAPH",
        "PARAGRAPH_CLOSE",
        "BOLD",
        "BOLD_CLOSE",
        "ITALIC",
        "ITALIC_CLOSE",
        "UNDERLINE",
        "UNDERLINE_CLOSE",
        "CITATION",
        "CITATION_CLOSE",
        "LIST",
        "LIST_CLOSE",
        "LIST_ITEM",
        "LIST_ITEM_CLOSE",
        "DOCUMENT",
        "DOCUMENT_CLOSE",
        "CONTENTS",
        "CONTENTS_CLOSE",
    };

    return strings[t];
}

struct TokenData {
    Token    tag;
    QString  content;
    uint32_t attributes;
};

#endif // PARSERTYPES_H
