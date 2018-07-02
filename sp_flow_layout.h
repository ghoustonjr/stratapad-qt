#ifndef SP_FLOW_LAYOUT_H
#define SP_FLOW_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

class sp_flow_layout : public QLayout
{
public:
    explicit sp_flow_layout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit sp_flow_layout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~sp_flow_layout();

    void addItem(QLayoutItem *item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QLayoutItem *takeAt(int index) override;

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif // SP_FLOW_LAYOUT_H
