#ifndef LINKTEXTOBJECT_H
#define LINKTEXTOBJECT_H

#include <QObject>
#include <QTextObjectInterface>

class LinkTextObject : public QObject, public QTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    QSizeF intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat &format) override;
    void drawObject(QPainter* painter, const QRectF &rect, QTextDocument* doc, int posInDocument, const QTextFormat &format) override;
};

#endif // LINKTEXTOBJECT_H
