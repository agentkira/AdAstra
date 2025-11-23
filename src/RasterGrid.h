#pragma once
#include <QGraphicsItem>
#include <QPixmap>

class RasterGrid : public QGraphicsItem {
public:
    RasterGrid();
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QPixmap m_cache;
    QSize   m_cacheSize;
    void ensureCache();
};
