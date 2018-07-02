#ifndef STRATAPADTOOLBUTTON_H
#define STRATAPADTOOLBUTTON_H

#include <QObject>
#include <QWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QButtonGroup>

class sp_tool_button : public QToolButton
{
    Q_OBJECT
public:
    sp_tool_button(QWidget* parent = 0);
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // STRATAPADTOOLBUTTON_H
