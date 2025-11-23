#include "Debris.h"
#include "Constants.h"

#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>





Debris::Debris()
{
    setZValue(7);

    auto* rng = QRandomGenerator::global();

    
    static const QVector<QPoint> dirs = {
        {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 },
        {  1, -1 }, { -1, -1 }, {  1,  1 }, { -1,  1 },
        {  2,  0 }, { -2,  0 }, {  0,  2 }, {  0, -2 }
    };

    QPoint d = dirs[rng->bounded(dirs.size())];
    m_stepX  = d.x();
    m_stepY  = d.y();

    m_life    = 8 + rng->bounded(5);  
    m_maxLife = m_life;
}

QRectF Debris::boundingRect() const
{
    const qreal s = Const::RASTER;
    return QRectF(-s / 2.0, -s / 2.0, s, s);
}

void Debris::paint(QPainter* p,
                   const QStyleOptionGraphicsItem*,
                   QWidget*)
{
    if (!p) return;

    p->setRenderHint(QPainter::Antialiasing, false);
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->setPen(Qt::NoPen);

    
    qreal t     = (m_maxLife > 0) ? (m_life / qreal(m_maxLife)) : 0.0;
    int   alpha = qBound(0, int(255 * t), 255);

    
    static const QColor colors[] = {
        QColor(255, 240, 160),
        QColor(255, 210, 110),
        QColor(255, 170,  80)
    };
    const QColor base = colors[m_life % 3];
    QColor c = base;
    c.setAlpha(alpha);

    p->setBrush(c);

    const qreal s = Const::RASTER;
    p->drawRect(QRectF(-s / 2.0, -s / 2.0, s, s));
}

void Debris::advance(int phase)
{
    if (phase == 0) return;

    
    setX(x() + m_stepX * Const::RASTER);
    setY(y() + m_stepY * Const::RASTER);

    if (--m_life <= 0) {
        deleteLater();
        return;
    }

    update();
}

