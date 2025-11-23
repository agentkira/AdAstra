#include "Mine.h"
#include "Constants.h"
#include "PixelArt.h"
#include "Game.h"
#include "Player.h"
#include "Explosion.h"
#include "HUD.h"

#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QtMath>
#include <QDateTime> 

namespace {

const QVector<QString> kMineSprite = {
    "....x....",
    ".........",
    "..x.g.x..",
    "...ggg...",
    "x.ggrgg.x",
    "...ggg...",
    "..x.g.x..",
    ".........",
    "....x...."
};

const QHash<QChar, QColor> kMinePalette = {
    { 'g', QColor(80, 80, 90) },    
    { 'r', QColor(255, 20, 20) },   
    { 'x', QColor(180, 180, 190) }  
};

} 

Mine::Mine() {
    setZValue(3);
    double r = QRandomGenerator::global()->generateDouble(); 
    m_sinePhase = r * (M_PI * 2.0);
}

QRectF Mine::boundingRect() const {
    const int w = kMineSprite[0].size() * Const::RASTER;
    const int h = kMineSprite.size() * Const::RASTER;
    return QRectF(-w / 2.0, -h / 2.0, w, h);
}

void Mine::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;
    QHash<QChar, QColor> pal = kMinePalette;
    
    
    int t = (QDateTime::currentMSecsSinceEpoch() / 100) % 2;
    if (t == 0) {
        pal['r'] = QColor(255, 100, 100); 
    } else {
        pal['r'] = QColor(150, 0, 0);     
    }
    drawPixelArt(p, kMineSprite, pal, true);
}

void Mine::advance(int phase) {
    if (phase == 0) return;

    Game* g = Game::instance();
    if (!g) return;

    m_sinePhase += Const::MINE_SINE_FREQ;
    qreal dx = std::sin(m_sinePhase) * Const::MINE_SINE_AMPL / 60.0;
    qreal dy = Const::MINE_SPEED * g->difficultySpeedMul();

    moveBy(dx, dy);

    if (y() > Const::SCENE_H + 50) {
        deleteLater();
        return;
    }

    const auto items = collidingItems();
    for (QGraphicsItem* it : items) {
        if (auto* player = dynamic_cast<Player*>(it)) {
            
            if (g->isShieldActive()) {
                explodeVigorously();
                
                return;
            }
            
            
            explodeVigorously();
            g->gameOver(); 
            return;
        }
    }
}

void Mine::explodeVigorously() {
    Game* g = Game::instance();
    QGraphicsScene* sc = scene();

    if (g && sc) {
        g->shakeScene(
            int(Const::SHAKE_MS * 2.0),
            Const::SHAKE_INTENSITY * 1.6
        );
        if (auto* hud = g->hud()) {
            hud->triggerFlash();
        }
    }

    if (sc) {
        
        Explosion* e1 = new Explosion(); e1->setPos(pos()); sc->addItem(e1);
        Explosion* e2 = new Explosion(); e2->setPos(pos() + QPointF(10, 10)); sc->addItem(e2);
        Explosion* e3 = new Explosion(); e3->setPos(pos() + QPointF(-10, -5)); sc->addItem(e3);
    }

    deleteLater();
}