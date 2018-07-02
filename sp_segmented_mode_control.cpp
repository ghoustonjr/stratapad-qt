#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QColor>
#include <QRect>
#include <QMouseEvent>
#include <QDebug>

#include "sp_segmented_mode_control.h"

void
TabOpacity::fadeIn()
{
    m_animator.stop();
    m_animator.setDuration(80);
    m_animator.setEndValue(255);
    m_animator.start();
}

void
TabOpacity::fadeOut()
{
    m_animator.stop();
    m_animator.setDuration(160);
    m_animator.setEndValue(0);
    m_animator.start();
}

sp_segmented_mode_control::sp_segmented_mode_control(QWidget* parent) : QWidget(parent)
{
    QTimer* timer = new QTimer(this);

    setMouseTracking(true);
    m_hoveringOver = -1;
    m_tabText      = {"Everything", "Explanations", "Data"};
    m_active       = Tab_Explanations;
    m_tabHeight     = 50;

    for (int tabIndex = 0; tabIndex < Tab_COUNT; tabIndex++)
    {
        m_animatedOpacities.append(new TabOpacity(this));
    }

    // TODO: Hmm, why did I do this again? Is it really needed, when I have all these events that I'm responding to?
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

QSize
sp_segmented_mode_control::minimumSizeHint() const
{
    int height = m_tabText.count() * m_tabHeight;
    return QSize(250, height);
}

QSize
sp_segmented_mode_control::sizeHint() const
{
    int height = m_tabText.count() * m_tabHeight;
    return QSize(250, height);
}

void
sp_segmented_mode_control::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QRect rect(0, 0, width(), height());

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.fillRect(rect, QColor(64, 66, 68));

    QFont boldFont(painter.font());
    boldFont.setPointSizeF(8.0f);
    boldFont.setBold(true);
    int textFlags = Qt::AlignCenter | Qt::AlignTop | Qt::TextWordWrap;

    QRect tabRect(0, 0, width(), m_tabHeight);
    QRect textRect(0, 0, width(), m_tabHeight);
    QColor activeTabColor = QColor(38, 40, 41);
    QColor hoverTabColor  = QColor(89, 91, 93);

    painter.setFont(boldFont);
    painter.setPen(QColor(255, 255, 255));
    for (int tabIndex = 0; tabIndex < Tab_COUNT; tabIndex++)
    {
        if (m_active != tabIndex)
        {
            QColor finalColor = hoverTabColor;
            finalColor.setAlpha(m_animatedOpacities.at(tabIndex)->opacity());
            painter.fillRect(tabRect, finalColor);
        }
        else
        {
            painter.fillRect(tabRect, activeTabColor);
        }

        painter.drawText(textRect, textFlags, m_tabText.at(tabIndex));
        painter.translate(0, m_tabHeight);
    }
}

void
sp_segmented_mode_control::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->x() >= 0) && (event->x() < width()))
    {
        if ((event->button() == Qt::LeftButton) && validTab(m_hoveringOver))
        {
            m_active = m_hoveringOver;
            emit tabIndexChanged(m_active);
        }
    }

    update();
    QWidget::mouseReleaseEvent(event);
}

void
sp_segmented_mode_control::mouseMoveEvent(QMouseEvent* event)
{
    int lastHover = m_hoveringOver;

    int yPos      = event->y();
    int nextHover = yPos / m_tabHeight;

    if ((event->y() < 0) || (event->y() > height()))
    {
        if (validTab(lastHover))
        {
            m_animatedOpacities.at(lastHover)->fadeOut();
        }

        m_hoveringOver = -1;
        return;
    }


    if (nextHover == lastHover)
    {
        return;
    }

    if (validTab(lastHover))
    {
        m_animatedOpacities.at(lastHover)->fadeOut();
    }

    if (validTab(nextHover))
    {
        m_animatedOpacities.at(nextHover)->fadeIn();
    }

    m_hoveringOver = nextHover;
    update();
}

void
sp_segmented_mode_control::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    if (validTab(m_hoveringOver))
    {
        m_animatedOpacities.at(m_hoveringOver)->fadeOut();
    }

    m_hoveringOver = -1;

    update();
}

bool
sp_segmented_mode_control::validTab(int idx)
{
    return (idx >= 0) && (idx < Tab_COUNT);
}
