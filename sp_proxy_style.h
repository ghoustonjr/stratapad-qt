#ifndef STRATAPADPROXYSTYLE_H
#define STRATAPADPROXYSTYLE_H

#include <QObject>
#include <QWidget>
#include <QProxyStyle>

class StratapadProxyStyle : public QProxyStyle
{
public:
    int
    pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const override
    {
        int ret = 0;
        switch (metric)
        {
            case QStyle::PM_ButtonShiftHorizontal:
            case QStyle::PM_ButtonShiftVertical:
                ret = 0;
                break;
            default:
                ret = QProxyStyle::pixelMetric(metric, option, widget);
                break;
        }
        return ret;
    }
};

#endif // STRATAPADPROXYSTYLE_H
