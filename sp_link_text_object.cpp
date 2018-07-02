#include <QFontMetrics>
#include <QPainter>
#include <QGuiApplication>
#include <QDebug>

#include "sp_textedit.h"
#include "sp_link_text_object.h"

QSizeF
LinkTextObject::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);

    QSizeF result;

    int linkNum = format.property(sp_textedit::CitationData).toInt();

    QString s = QString("[%1]").arg(linkNum);
    QFontMetrics fm(QGuiApplication::font());

    result.setWidth(fm.width(s));
    result.setHeight(fm.height() + 2.5);

    return result;
}

void
LinkTextObject::drawObject(QPainter* painter, const QRectF &rect, QTextDocument* doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);

    painter->setFont(QGuiApplication::font());
    painter->setPen(QColor(153, 51, 51));
    int flags = Qt::AlignTop;
    painter->drawText(rect, flags, QString("[*]"));
}
