#include "sp_tool_button.h"

sp_tool_button::sp_tool_button(QWidget* parent) : QToolButton(parent){ }

void
sp_tool_button::mouseReleaseEvent(QMouseEvent* event)
{
    if (group() && isCheckable())
    {
        if ((group()->checkedId()) == (group()->id(this)))
        {
            group()->setExclusive(false);
        }
    }

    QToolButton::mouseReleaseEvent(event);
    group()->setExclusive(true);
}
