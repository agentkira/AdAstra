#include "PowerUp.h"
#include "Constants.h"
#include "PixelArt.h"
#include "Game.h" 
#include "Player.h" 
#include <QPainter>
#include <QtMath>

namespace {

const QVector<QString> kShieldLetter = {
    ".#####.", 
    "#.....#", 
    "#..____", 
    ".#####.", 
    "____..#", 
    "#.....#", 
    ".#####."
};
const QVector<QString> kTriShotLetter = {
    "#######", 
    "...#...", 
    "...#...", 
    "...#...", 
    "...#...", 
    "...#...", 
    "...#..."
};
const QVector<QString> kHealthLetter = {
    "#.....#", 
    "#.....#", 
    "#######", 
    "#######", 
    "#.....#", 
    "#.....#", 
    "#.....#"
};
}

PowerUp::PowerUp(Type t) : m_type(t) {
    setZValue(Const::Z_POWERUPS);
    m_sinePhase = 0;
}

QRectF PowerUp::boundingRect() const {
    return QRectF(-16, -16, 32, 32);
}

void PowerUp::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if(!p) return;
    p->setRenderHint(QPainter::Antialiasing, false);

    QColor c;
    const QVector<QString>* sprite = nullptr;

    switch(m_type) {
    case Shield:    c = QColor(80, 180, 255); sprite = &kShieldLetter; break; 
    case TriShot:   c = QColor(255, 200, 0);  sprite = &kTriShotLetter; break; 
    case HealthPack:c = QColor(50, 220, 50);  sprite = &kHealthLetter; break;  
    }

    int size = 28; 
    QRect boxRect(-size/2, -size/2, size, size);
    
    
    p->setPen(QPen(Qt::white, 2));
    p->setBrush(c.darker(200)); 
    p->drawRect(boxRect);

    
    p->setPen(c);
    p->setBrush(Qt::NoBrush);
    p->drawRect(boxRect.adjusted(2, 2, -2, -2));

    
    QHash<QChar, QColor> pal = {{'#', Qt::white}, {'_', Qt::white}};
    if (sprite) {
        drawPixelArt(p, *sprite, pal, true);
    }
}

void PowerUp::advance(int phase) {
    if (phase == 0) return;

    setY(y() + Const::POWERUP_SPEED);
    m_sinePhase += 0.1;

    
    const auto items = collidingItems();
    for (QGraphicsItem* item : items) {
        if (dynamic_cast<Player*>(item)) {
            if (Game::instance()) {
                Game::instance()->activatePowerUp(m_type);
                deleteLater(); 
                return;      
            }
        }
    }

    if (y() > Const::SCENE_H + 50) {
        deleteLater();
    }
}