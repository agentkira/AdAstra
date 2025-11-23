#include "RasterGrid.h"
#include "Constants.h"
#include <QPainter>
#include <QPen>

RasterGrid::RasterGrid() {
    setZValue(-10);
    setAcceptedMouseButtons(Qt::NoButton);
}

QRectF RasterGrid::boundingRect() const {
    return QRectF(0, 0, Const::SCENE_W, Const::SCENE_H);
}

void RasterGrid::ensureCache() {
    const QSize sz(Const::SCENE_W, Const::SCENE_H);
    if (m_cacheSize == sz && !m_cache.isNull()) return;

    m_cache = QPixmap(sz);
    m_cache.fill(Qt::transparent);

    QPainter p(&m_cache);
    p.setRenderHint(QPainter::Antialiasing, false);

    QPen pen(QColor(200,200,200,100));
    pen.setWidth(1);
    p.setPen(pen);
    const qreal offset = 0.5;

    for (int x = 0; x <= Const::SCENE_W; x += Const::RASTER)
        p.drawLine(QPointF(x + offset, 0), QPointF(x + offset, Const::SCENE_H));
    for (int y = 0; y <= Const::SCENE_H; y += Const::RASTER)
        p.drawLine(QPointF(0, y + offset), QPointF(Const::SCENE_W, y + offset));

    QPen border(QColor(120,120,120,150));
    border.setWidth(1);
    p.setPen(border);
    p.drawRect(QRectF(0.5, 0.5, Const::SCENE_W - 1.0, Const::SCENE_H - 1.0));

    m_cacheSize = sz;
}

void RasterGrid::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    ensureCache();
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->drawPixmap(0, 0, m_cache);
}
