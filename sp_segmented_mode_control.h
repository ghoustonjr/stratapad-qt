#ifndef SEGMENTEDMODECONTROL_H
#define SEGMENTEDMODECONTROL_H

#include <QObject>
#include <QWidget>
#include <QSize>
#include <QPropertyAnimation>

class TabOpacity : public QWidget {
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    TabOpacity(QWidget* parent)
    {
        m_bar     = parent;
        m_opacity = 0;
        m_animator.setPropertyName("opacity");
        m_animator.setTargetObject(this);
    }

    float
    opacity(){ return m_opacity; }

    void
    setOpacity(float x){ m_opacity = x; m_bar->update();  }

    void fadeIn();
    void fadeOut();

    QPropertyAnimation m_animator;
    float              m_opacity;
    QWidget*           m_bar;
};

class sp_segmented_mode_control : public QWidget
{
    Q_OBJECT

    enum Tabs {
        Tab_All = 0,
        Tab_Explanations,
        Tab_Observations,

        Tab_COUNT
    };

public:
    explicit
    sp_segmented_mode_control(QWidget* parent = 0);

protected:
    QRect tabRect(int index, int tabSideLength) const;
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);

    bool validTab(int idx);

    QList<TabOpacity *> m_animatedOpacities;
    int                 m_active;
    int                 m_hoveringOver;
    int                 m_tabHeight;
    QList<QString>      m_tabText;

signals:
    void tabIndexChanged(int idx);
};

#endif // SEGMENTEDMODECONTROL_H
