#include <QHBoxLayout>
#include <QTextCursor>
#include <QDateTime>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextBlock>
#include <QBrush>

#include "sp_log_view.h"

LogView::LogView(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_log = new QTextBrowser();
    layout->addWidget(m_log);
}

void
LogView::addLogEntry(QString stateSHA, int timestamp, QString bodyAsHtml)
{
    QTextDocument body;
    QString time = QDateTime::fromSecsSinceEpoch(timestamp).toString();

    QFont font("Century Schoolbook");
    font.setPointSize(14);
    body.setDefaultFont(font);

    body.setHtml(bodyAsHtml);

    QBrush rootBackgroundBrush;
    rootBackgroundBrush.setStyle(Qt::SolidPattern);
    rootBackgroundBrush.setColor(QColor(240, 240, 240));

    QTextFrameFormat rootFormat;
    rootFormat.setBackground(rootBackgroundBrush);
    body.rootFrame()->setFrameFormat(rootFormat);

    QTextCursor cursor(m_log->textCursor());
    cursor.movePosition(QTextCursor::End);

    QTextFrameFormat frameFormat;
    frameFormat.setLeftMargin(100);
    frameFormat.setRightMargin(100);

    QBrush backgroundBrush;
    backgroundBrush.setStyle(Qt::SolidPattern);
    backgroundBrush.setColor(QColor(252, 252, 252));
    frameFormat.setBackground(backgroundBrush);
    frameFormat.setBorder(1);
    frameFormat.setPadding(5);

    QTextCharFormat timeFormat;
    timeFormat.setFontPointSize(10);
    // timeFormat.setFontUnderline(true);
    timeFormat.setFontWeight(QFont::Bold);
    timeFormat.setAnchor(true);
    timeFormat.setAnchorHref(QString("#").append(stateSHA));
    timeFormat.setAnchorName(stateSHA);

    cursor.insertFrame(frameFormat);
    cursor.insertText(time, timeFormat);

    QTextCursor fmtCursor         = QTextCursor(cursor);
    QTextBlockFormat timeBlockFmt = fmtCursor.block().blockFormat();
    timeBlockFmt.setBottomMargin(7);
    fmtCursor.setBlockFormat(timeBlockFmt);

    cursor.insertBlock();
    cursor.insertFragment(QTextDocumentFragment(&body));

    QTextCursor scrollCursor = m_log->textCursor();
    scrollCursor.movePosition(QTextCursor::End);
    m_log->setTextCursor(scrollCursor);
}
