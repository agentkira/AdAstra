#include "Star.h"
#include "Constants.h"
#include "PixelArt.h"
#include <QRandomGenerator>

Star::Star(Layer l) : m_layer(l) {
    setZValue(0);
    auto* rng = QRandomGenerator::global();
    setPos(rng->bounded(Const::SCENE_W), rng->bounded(Const::SCENE_H));
    m_speed = (m_layer == Layer::Back) ? 0.5 * Const::RASTER : 1.0 * Const::RASTER;
}

QRectF Star::boundingRect() const {
    return QRectF(-Const::RASTER/2.0, -Const::RASTER/2.0, Const::RASTER, Const::RASTER);
}

void Star::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setRenderHint(QPainter::Antialiasing, false);
    p->fillRect(QRectF(-Const::RASTER/2.0, -Const::RASTER/2.0, Const::RASTER, Const::RASTER),
                (m_layer == Layer::Front) ? QColor(255,255,255) : QColor(200,200,200));
}

void Star::advance(int phase) {
    if (phase == 0) return;
    m_accum += m_speed;
    while (m_accum >= Const::RASTER) {
        setY(y() + Const::RASTER);
        m_accum -= Const::RASTER;
    }
    if (y() > Const::SCENE_H) setY(0);
}
