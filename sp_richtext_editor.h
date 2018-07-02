#ifndef RICH_TEXT_EDITOR_H
#define RICH_TEXT_EDITOR_H

#include <QObject>
#include <QWidget>
#include <QToolBar>
#include <QTextEdit>
#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QToolButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QTextList>
#include <QButtonGroup>
#include <QLabel>

#include "sp_textedit.h"
#include "sp_tool_button.h"

class sp_richtext_editor : public QMainWindow {
    Q_OBJECT

public:

    explicit
    sp_richtext_editor(QWidget* parent = 0);

    //bool eventFilter(QObject* watched, QEvent* event) override;

    void AddDropShadow(QWidget* widget);
    void SetAcceptRichText(bool accept);
    void MergeFormat(const QTextCharFormat &format);

    sp_textedit *SPTextEdit;

    QToolBar *TextBar;
    QToolBar *HeaderBar;
    QToolBar *BlockBar;
    QToolBar *AlignBar;
    QToolBar *ActionBar;
    QToolButton *Bold;
    QToolButton *Italic;
    QToolButton *Underline;
    QToolButton *Header;
    QToolButton *Indent;
    QToolButton *Dedent;
    QToolButton *AlignLeft;
    QToolButton *AlignCenter;
    QToolButton *AlignRight;
    QToolButton *AlignJustify;
    QToolButton *InsertPic;
    QToolButton *InsertLink;
    QToolButton *InsertCite;
    QButtonGroup *ListGroup;
    QButtonGroup *AlignGroup;
    sp_tool_button* UnorderedList;
    sp_tool_button* OrderedList;

public slots:
    void ResetToolbar();
    void SetToolbarEnabled(bool enabled);
    void SetBlockAlignment();
    void UpdateBlockToolbar();
    void UpdateTextToolbar(const QTextCharFormat &format);

    void Clear();
    void ReturnToEditor();
    void GiveEditorFocus();
    void UpdateCitationNumbers();
    void CreateCitation(const QString& claimSHA);

    QString XML();
    void SetXML(const QString& XML);

    void SwitchDoc(QTextDocument *Doc);

signals:
    void EditorContentsChanged(const QString& xml);
    void AddCitation();
    void MakeClaim(const QString& claim);
};

#endif
