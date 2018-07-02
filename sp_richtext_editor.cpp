#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QTextFragment>
#include <QGraphicsDropShadowEffect>
#include <QTextDocumentFragment>
#include <QDateTime>
#include <QTimer>
#include <QScreen>
#include <QButtonGroup>

#include "sp_parser.h"
#include "sp_reader.h"
#include "sp_writer.h"
#include "sp_richtext_editor.h"
#include "sp_util.h"


sp_richtext_editor::sp_richtext_editor(QWidget* parent)
    : QMainWindow(parent)
{
    // ============
    // EDITOR SETUP
    // ============
    // CR-TODO: Can this fail? And do I need to load this here, or could I load it at application startup?
    QFile stylesheet(":/css/rich_text_editor.css");
    stylesheet.open(QFile::ReadOnly);
    QString styles = QString(stylesheet.readAll());
    this->setStyleSheet(styles);

    SPTextEdit = new sp_textedit(this);
    SPTextEdit->installEventFilter(this);

    this->setCentralWidget(SPTextEdit);
    this->setWindowFlags(Qt::Widget);
    this->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    // ========================
    // FORMATTING TOOLBAR SETUP
    // ========================

    TextBar = this->addToolBar("Edit Actions");
    TextBar->setObjectName("text-format-bar");
    AddDropShadow(TextBar);
    TextBar->installEventFilter(this);

    // CR-TODO: Load these based upon device pixel ratio
    QIcon boldIcon(":/icons/bold@2.png");
    boldIcon.addFile("://icons/white/bold@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon italicsIcon("://icons/italic@2.png");
    italicsIcon.addFile("://icons/white/italic@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon underlineIcon("://icons/underline@2.png");
    underlineIcon.addFile("://icons/white/underline@2.png", QSize(), QIcon::Normal, QIcon::On);

    Bold = new QToolButton(this);
    Bold->setObjectName("bold-button");

    Italic = new QToolButton(this);
    Italic->setObjectName("italic-button");

    Underline = new QToolButton(this);
    Underline->setObjectName("underline-button");

    Bold->setIcon(boldIcon);
    Bold->setCheckable(true);

    Italic->setIcon(italicsIcon);
    Italic->setCheckable(true);

    Underline->setIcon(underlineIcon);
    Underline->setCheckable(true);

    TextBar->setFloatable(false);
    TextBar->setMovable(false);
    TextBar->addWidget(Bold);
    TextBar->addWidget(Italic);
    TextBar->addWidget(Underline);
    TextBar->setIconSize(QSize(16, 16));

    connect(Bold, &QToolButton::clicked, SPTextEdit, &sp_textedit::textBold);
    connect(Underline, &QToolButton::clicked, SPTextEdit, &sp_textedit::textUnderline);
    connect(Italic, &QToolButton::clicked, SPTextEdit, &sp_textedit::textItalic);

    // ===========================
    // HEADER FORMAT TOOLBAR SETUP
    // ===========================
    HeaderBar = this->addToolBar("Headings");

    QGraphicsDropShadowEffect* headerFormatShadow = new QGraphicsDropShadowEffect();
    headerFormatShadow->setBlurRadius(1); // Adjust accordingly
    headerFormatShadow->setOffset(0, 1);  // Adjust accordingly
    headerFormatShadow->setColor(QColor(63, 63, 63, 50));
    HeaderBar->setGraphicsEffect(headerFormatShadow);
    HeaderBar->installEventFilter(this);

    QIcon headerIcon("://icons/header@2.png");
    headerIcon.addFile("://icons/white/header@2.png", QSize(), QIcon::Normal, QIcon::On);

    Header = new QToolButton(this);
    Header->setObjectName("header-button");
    Header->setIcon(headerIcon);
    Header->setCheckable(true);

    HeaderBar->setMovable(false);
    HeaderBar->addWidget(Header);
    HeaderBar->setIconSize(QSize(16, 16));

    // ==========================
    // BLOCK FORMAT TOOLBAR SETUP
    // ==========================
    BlockBar = this->addToolBar("Block Formatting");
    AddDropShadow(BlockBar);
    BlockBar->installEventFilter(this);

    QIcon ulistIcon("://icons/list-ul@2.png");
    ulistIcon.addFile("://icons/white/list-ul@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon olistIcon("://icons/list-ol@2.png");
    olistIcon.addFile("://icons/white/list-ol@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon indentIcon("://icons/indent@2.png");
    QIcon dedentIcon("://icons/dedent@2.png");

    ListGroup = new QButtonGroup(this);
    ListGroup->setExclusive(true);

    UnorderedList = new sp_tool_button(this);
    UnorderedList->setObjectName("ulist-button");
    UnorderedList->setCheckable(true);
    UnorderedList->setIcon(ulistIcon);

    OrderedList = new sp_tool_button(this);
    OrderedList->setObjectName("olist-button");
    OrderedList->setCheckable(true);
    OrderedList->setIcon(olistIcon);

    ListGroup->addButton(UnorderedList);
    ListGroup->addButton(OrderedList);

    Indent = new QToolButton(this);
    Indent->setObjectName("indent-button");
    Indent->setIcon(indentIcon);

    InsertLink = new QToolButton(this);
    InsertLink->setObjectName("dedent-button");
    InsertLink->setIcon(dedentIcon);

    BlockBar->setFloatable(false);
    BlockBar->setMovable(false);
    BlockBar->addWidget(UnorderedList);
    BlockBar->addWidget(OrderedList);
    BlockBar->addWidget(Indent);
    BlockBar->addWidget(InsertLink);
    BlockBar->setIconSize(QSize(16, 16));

    connect(UnorderedList, &sp_tool_button::clicked, SPTextEdit, &sp_textedit::handleUnorderedList);
    connect(OrderedList, &sp_tool_button::clicked, SPTextEdit, &sp_textedit::handleOrderedList);

    // ==========================
    // ALIGN BUTTON TOOLBAR SETUP
    // ==========================
    AlignBar = this->addToolBar("Block Alignment");
    AddDropShadow(AlignBar);
    AlignBar->installEventFilter(this);

    QIcon alignLeftIcon(":/icons/align-left@2.png");
    alignLeftIcon.addFile(":/icons/white/align-left@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon alignCenterIcon(":/icons/align-center@2.png");
    alignCenterIcon.addFile(":/icons/white/align-center@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon alignRightIcon(":/icons/align-right@2.png");
    alignRightIcon.addFile(":/icons/white/align-right@2.png", QSize(), QIcon::Normal, QIcon::On);

    QIcon alignJustifyIcon(":/icons/align-justify@2.png");
    alignJustifyIcon.addFile(":/icons/white/align-justify@2.png", QSize(), QIcon::Normal, QIcon::On);

    AlignGroup = new QButtonGroup(this);
    AlignGroup->setExclusive(true);

    AlignLeft = new QToolButton(this);
    AlignLeft->setObjectName("align-left-button");
    AlignLeft->setCheckable(true);
    AlignLeft->setIcon(alignLeftIcon);
    AlignLeft->setChecked(true);

    AlignCenter = new QToolButton(this);
    AlignCenter->setObjectName("align-center-button");
    AlignCenter->setCheckable(true);
    AlignCenter->setIcon(alignCenterIcon);

    AlignRight = new QToolButton(this);
    AlignRight->setObjectName("align-right-button");
    AlignRight->setCheckable(true);
    AlignRight->setIcon(alignRightIcon);

    AlignJustify = new QToolButton(this);
    AlignJustify->setObjectName("align-justify-button");
    AlignJustify->setCheckable(true);
    AlignJustify->setIcon(alignJustifyIcon);

    AlignGroup->addButton(AlignLeft);
    AlignGroup->addButton(AlignCenter);
    AlignGroup->addButton(AlignRight);
    AlignGroup->addButton(AlignJustify);

    AlignBar->setFloatable(false);
    AlignBar->setMovable(false);
    AlignBar->addWidget(AlignLeft);
    AlignBar->addWidget(AlignCenter);
    AlignBar->addWidget(AlignRight);
    AlignBar->addWidget(AlignJustify);
    AlignBar->setIconSize(QSize(16, 16));

    connect(AlignLeft, &QToolButton::clicked, this, &sp_richtext_editor::SetBlockAlignment);
    connect(AlignRight, &QToolButton::clicked, this, &sp_richtext_editor::SetBlockAlignment);
    connect(AlignCenter, &QToolButton::clicked, this, &sp_richtext_editor::SetBlockAlignment);
    connect(AlignJustify, &QToolButton::clicked, this, &sp_richtext_editor::SetBlockAlignment);

    // ===========================
    // ACTION BUTTON TOOLBAR SETUP
    // ===========================
    ActionBar = this->addToolBar("Actions");
    AddDropShadow(ActionBar);
    ActionBar->installEventFilter(this);

    QIcon insertPicIcon(":/icons/image@2.png");
    QIcon insertLinkIcon(":/icons/link@2.png");

    InsertPic = new QToolButton(this);
    InsertPic->setObjectName("insert-pic-button");
    InsertPic->setIcon(insertPicIcon);

    InsertLink = new QToolButton(this);
    InsertLink->setObjectName("insert-link-button");
    InsertLink->setIcon(insertLinkIcon);

    ActionBar->setFloatable(false);
    ActionBar->setMovable(false);
    ActionBar->addWidget(InsertPic);
    ActionBar->addWidget(InsertLink);
    ActionBar->setIconSize(QSize(16, 16));

    // All toolbars are disabled until the editor has focus
    SetToolbarEnabled(true);

    // ================
    // MISC CONNECTIONS
    // ================

    connect(SPTextEdit, &sp_textedit::requestMakeClaim, this, &sp_richtext_editor::MakeClaim);
    connect(SPTextEdit, &sp_textedit::requestAddCitation, this, &sp_richtext_editor::AddCitation);

    connect(SPTextEdit, &QTextEdit::cursorPositionChanged, this, &sp_richtext_editor::UpdateBlockToolbar);
    connect(SPTextEdit, &QTextEdit::textChanged, this, &sp_richtext_editor::UpdateBlockToolbar);
    connect(SPTextEdit, &QTextEdit::currentCharFormatChanged, this, &sp_richtext_editor::UpdateTextToolbar);

    SetAcceptRichText(false);
}

void
sp_richtext_editor::AddDropShadow(QWidget* widget)
{
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(1);
    shadow->setOffset(0, 1);
    shadow->setColor(QColor(63, 63, 63, 50));

    widget->setGraphicsEffect(shadow);
}

// CR-TODO: Move this into the class that displays notes when they're not being edited.
void
sp_richtext_editor::UpdateCitationNumbers()
{
    QTextDocument* doc = SPTextEdit->document();

    int lastCitationNum = 0;

    for (QTextBlock b = doc->begin(); b != doc->end(); b = b.next())
    {
        QTextBlock::iterator it;
        for (it = b.begin(); !it.atEnd(); ++it)
        {
            QTextFragment frag = it.fragment();
            if (frag.isValid())
            {
                QTextCharFormat fragFmt = frag.charFormat();
                if (fragFmt.objectType() == sp_textedit::CiteObjectType)
                {
                    QTextCharFormat updatedFmt = fragFmt;
                    updatedFmt.setProperty(sp_textedit::CitationData, ++lastCitationNum);

                    int pos = frag.position();
                    int len = frag.length();

                    qDebug() << pos << len;

                    QTextCursor c = SPTextEdit->textCursor();
                    c.setPosition(frag.position());
                    c.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    c.setCharFormat(updatedFmt);
                }
            }
        }
    }
}

void
sp_richtext_editor::UpdateTextToolbar(const QTextCharFormat &format)
{
    if (format.isBlockFormat())
    {
        qDebug() << "This is a block format too.";
    }

    QFontInfo fontInfo(format.font());

    Bold->setChecked(fontInfo.bold());
    Italic->setChecked(fontInfo.italic());
    Underline->setChecked(fontInfo.underline());
}

void
sp_richtext_editor::SetBlockAlignment()
{
    QObject* sender = QObject::sender();

    if (sender)
    {
        Qt::Alignment align;
        if (sender == AlignLeft)
        {
            align = Qt::AlignLeft;
        }
        else if (sender == AlignCenter)
        {
            align = Qt::AlignCenter;
        }
        else if (sender == AlignRight)
        {
            align = Qt::AlignRight;
        }
        else if (sender == AlignJustify)
        {
            align = Qt::AlignJustify;
        }
        else
        {
            return;
        }

        QTextCursor c = SPTextEdit->textCursor();

        QTextBlockFormat fmt = c.block().blockFormat();
        fmt.setAlignment(align);
        c.mergeBlockFormat(fmt);
    }
}

void
sp_richtext_editor::UpdateBlockToolbar()
{
    QTextCursor c = SPTextEdit->textCursor();

    bool orList = false;
    bool unList = false;

    QTextList* list = c.block().textList();
    if (list)
    {
        QTextListFormat::Style s = list->format().style();
        orList = (QTextListFormat::ListDecimal == s);
        unList = (QTextListFormat::ListDisc == s);
    }

    ListGroup->setExclusive(false);
    UnorderedList->setChecked(unList);
    OrderedList->setChecked(orList);
    ListGroup->setExclusive(true);

    QTextBlockFormat fmt = c.block().blockFormat();
    switch (fmt.alignment())
    {
        case Qt::AlignLeft:
        {
            AlignLeft->setChecked(true);
        } break;

        case Qt::AlignCenter:
        {
            AlignCenter->setChecked(true);
        } break;

        case Qt::AlignRight:
        {
            AlignRight->setChecked(true);
        } break;

        case Qt::AlignJustify:
        {
            AlignJustify->setChecked(true);
        } break;
    }
}

void
sp_richtext_editor::ResetToolbar()
{
    Bold->setChecked(false);
    Italic->setChecked(false);
    Underline->setChecked(false);
}

void
sp_richtext_editor::SetToolbarEnabled(bool enabled)
{
    TextBar->setEnabled(enabled);
    HeaderBar->setEnabled(enabled);
    AlignBar->setEnabled(enabled);
    BlockBar->setEnabled(enabled);
    ActionBar->setEnabled(enabled);
}

// =====
// MISC.
// =====

#if 0
// CR-TODO: Figure out how to make this work. It works generally, but it will also need to work with the menu bar's formatting menu.
bool
sp_richtext_editor::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);
    switch (event->type())
    {
        case QEvent::FocusIn:
        {
            QTextCharFormat fmt = SPTextEdit->textCursor().charFormat();
            UpdateTextToolbar(fmt);
            SetToolbarEnabled(true);
        } break;

        case QEvent::FocusOut:
        {
            ResetToolbar();
            SetToolbarEnabled(false);
        } break;
    }

    return false;
}
#endif

void
sp_richtext_editor::CreateCitation(const QString &claimSHA)
{
    SPTextEdit->insertCitation(claimSHA);
}

void
sp_richtext_editor::SetAcceptRichText(bool accept)
{
    SPTextEdit->setAcceptRichText(accept);
}

void
sp_richtext_editor::Clear()
{
    SPTextEdit->clear();
}

void
sp_richtext_editor::ReturnToEditor()
{
    SPTextEdit->setFocus();
}

void
sp_richtext_editor::GiveEditorFocus()
{
    SPTextEdit->setFocus();

    QTextCursor cursor = SPTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);

    SPTextEdit->setTextCursor(cursor);
}

QString
sp_richtext_editor::XML()
{
    return QString();
}

void
sp_richtext_editor::SetXML(const QString& xml)
{
    SPTextEdit->clear();
    {
        ReaderXML t(xml);
        WriterMem s(SPTextEdit->document());
        StratapadParser p(&t, &s);
        p.parseDocument();
    }
}

void
sp_richtext_editor::SwitchDoc(QTextDocument *Doc)
{
    SPTextEdit->setDocument(Doc);


}
