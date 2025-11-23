#include "Planet.h"
#include "Constants.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>

Planet::Planet(BiomeType type) : m_type(type) {
    setZValue(Const::Z_PLANETS);
    auto* rng = QRandomGenerator::global();
    m_speed = Const::PLANET_SPEED_MIN + rng->generateDouble() * 0.3;
    generateSprite();
    int rawX = rng->bounded(0, Const::SCENE_W);
    setPos((rawX / Const::RASTER) * Const::RASTER, -boundingRect().height());
}

QRectF Planet::boundingRect() const {
    return QRectF(0, 0, m_sprite.width(), m_sprite.height());
}

void Planet::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;
    p->drawPixmap(0, 0, m_sprite);
}

void Planet::advance(int phase) {
    if (phase == 0) return;
    
    
    if (m_exiting) {
        setY(y() + 8.0); 
    } else {
        setY(y() + m_speed);
    }

    if (y() > Const::SCENE_H + 200) {
        deleteLater();
    }
}


void Planet::accelerateExit() {
    m_exiting = true;
}

void Planet::generateSprite() {
    
    
    auto* rng = QRandomGenerator::global();
    int cells = rng->bounded(24, 50);
    if (cells % 2 != 0) cells++;
    bool hasRings = (rng->bounded(4) == 0);
    int canvasSize = hasRings ? int(cells * 1.6) : cells;
    int w = canvasSize * Const::RASTER;
    m_sprite = QPixmap(w, w);
    m_sprite.fill(Qt::transparent);
    QPainter p(&m_sprite);
    p.setRenderHint(QPainter::Antialiasing, false);

    QColor cBase, cDark, cDetail;
    switch (m_type) {
    case BiomeType::Frost:  cBase=QColor(140,190,220); cDark=QColor(70,110,170); cDetail=QColor(210,230,250); break;
    case BiomeType::Fire:   cBase=QColor(170,70,50);   cDark=QColor(90,30,20);   cDetail=QColor(210,130,40); break;
    case BiomeType::Jungle: cBase=QColor(50,130,70);   cDark=QColor(25,65,35);   cDetail=QColor(90,170,190); break;
    case BiomeType::Dust:   cBase=QColor(150,130,100); cDark=QColor(90,80,60);   cDetail=QColor(180,170,160); break;
    case BiomeType::Ocean:  cBase=QColor(35,70,150);   cDark=QColor(15,30,90);   cDetail=QColor(70,130,190); break;
    case BiomeType::Cyber:  
    default:                cBase=QColor(90,30,130);   cDark=QColor(40,15,70);   cDetail=QColor(0,210,210); break;
    }

    double radius = cells / 2.0;
    double center = w / (2.0 * Const::RASTER);
    double rSq = radius * radius;

    for (int y = 0; y < canvasSize; ++y) {
        for (int x = 0; x < canvasSize; ++x) {
            double dx = (x + 0.5) - center;
            double dy = (y + 0.5) - center;
            if (dx*dx + dy*dy <= rSq) {
                QColor final = cBase;
                if ((x*53+y*97)%100 > 75) final = cDetail;
                else if ((x*53+y*97)%100 < 25) final = cDark;
                if (x > center + radius*0.3 || y > center + radius*0.3) {
                    if ((x + y) % 2 == 0) final = final.darker(140);
                }
                p.fillRect(x * Const::RASTER, y * Const::RASTER, Const::RASTER, Const::RASTER, final);
            }
        }
    }
    if (hasRings) {
        QColor rc = cDetail.lighter(115);
        for (int y = 0; y < canvasSize; ++y) {
            for (int x = 0; x < canvasSize; ++x) {
                double dx = (x+0.5)-center; double dy = (y+0.5)-center;
                double dr = dx*dx + (dy*3.0)*(dy*3.0);
                if (dr > (radius+2)*(radius+2) && dr < (radius+6)*(radius+6)) {
                    if (!((y < center) && (dx*dx+dy*dy < rSq))) {
                        if ((x+y)%2!=0) p.fillRect(x*Const::RASTER, y*Const::RASTER, Const::RASTER, Const::RASTER, rc);
                    }
                }
            }
        }
    }
}