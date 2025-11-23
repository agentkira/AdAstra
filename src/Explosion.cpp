#include "Explosion.h"
#include "Constants.h"
#include "Debris.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QtMath>

Explosion::Explosion() {
    setZValue(8);
}

QRectF Explosion::boundingRect() const {
    
    const qreal r = Const::RASTER * 6;
    return QRectF(-r, -r, 2 * r, 2 * r);
}

void Explosion::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;

    
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->setRenderHint(QPainter::TextAntialiasing, false);

    const qreal t  = m_frame / qreal(Const::EXP_FRAMES); 
    const qreal t2 = t * t;

    
    const int coreR = Const::RASTER * 2;
    QColor coreCol(255, 240, 180);
    coreCol.setAlpha(int(255 * (1.0 - t)));
    p->setPen(Qt::NoPen);
    p->setBrush(coreCol);
    p->drawRect(-coreR, -coreR, coreR * 2, coreR * 2);

    
    const qreal maxR = Const::RASTER * 5;
    const qreal r = (Const::RASTER * 1.5)
                    + t * (maxR - Const::RASTER * 1.5);

    const int chunkSize = Const::RASTER * 2;
    QColor chunkCol(255, 180, 70);
    chunkCol.setAlpha(int(220 * (1.0 - t2)));
    p->setBrush(chunkCol);

    for (int i = 0; i < 8; ++i) {
        qreal ang = (M_PI / 4.0) * i;
        qreal cx = std::cos(ang) * r;
        qreal cy = std::sin(ang) * r;

        p->drawRect(QRectF(cx - chunkSize / 2.0,
                           cy - chunkSize / 2.0,
                           chunkSize,
                           chunkSize));
    }

    
    QRandomGenerator* rng = QRandomGenerator::global();
    QColor sparkCol(255, 255, 210, int(160 * (1.0 - t)));
    p->setBrush(sparkCol);

    const int gridRadius = 5;
    for (int i = 0; i < 6; ++i) {
        int gx = rng->bounded(-gridRadius, gridRadius + 1) * Const::RASTER;
        int gy = rng->bounded(-gridRadius, gridRadius + 1) * Const::RASTER;
        p->drawRect(gx, gy, Const::RASTER, Const::RASTER);
    }
}

void Explosion::advance(int phase) {
    if (phase == 0) return;

    
    if (!m_debrisSpawned && m_frame >= 2) {
        m_debrisSpawned = true;
        if (auto* sc = scene()) {
            for (int i = 0; i < Const::EXP_DEBRIS; ++i) {
                auto* d = new Debris();
                d->setPos(pos());
                sc->addItem(d);
            }
        }
    }

    ++m_frame;
    if (m_frame >= Const::EXP_FRAMES) {
        deleteLater();
        return;
    }

    update();
}


