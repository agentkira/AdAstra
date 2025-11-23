#include "Asteroid.h"
#include "Constants.h"
#include "PixelArt.h"
#include "Game.h"
#include "Explosion.h"
#include "Debris.h"
#include "Player.h" 
#include <QGraphicsScene>
#include <QPainter>
#include <QRandomGenerator>
#include <QtMath>

namespace {
const QVector<QVector<QString>> kAsteroidSpritesLarge = {
    {".....###.....","...##ooo##...","..#ooooooo#..",".#ooo***ooo#.",".#oo*****oo#.",
     "#ooo*****ooo#","#ooooooooooo#",".#ooooooooo#."," .#ooooooooo#.",
     "..#ooooooo#..","...#######...",".....###....."},
    {"....###......","..##ooo##....",".#ooooooo#...","#oo****ooo#..","#oo*****oo#..",
     "#oooooooooo#.",".#oooooooooo#","..#oooooooo#.","...#######...",".....###....."}
};
const QVector<QVector<QString>> kAsteroidSpritesMedium = {
    {"...##..",".##oo#.","#oo**o#","#ooooo#",".#ooo#.", "..###.."},
    {"..###..",".#oo*#.","#ooooo#",".#ooo#.", "..###.."}
};
const QVector<QVector<QString>> kAsteroidSpritesSmall = {
    {".#.", "#o#", ".#."}, {".#.", "#*#", "###"}
};
const QHash<QChar, QColor> kAsteroidPalette = {
    { '#', QColor(100, 90, 80) },
    { 'o', QColor(140, 130, 120) },
    { '*', QColor(80, 70, 60) }
};
}

Asteroid::Asteroid(Size s) : m_size(s) {
    setZValue(Const::Z_ENTITIES);
    m_life.start();

    auto* rng = QRandomGenerator::global();
    if (m_size == Large) m_speed = Const::AST_MIN_SPEED + rng->generateDouble()*0.5;
    else if (m_size == Medium) m_speed = Const::AST_MIN_SPEED + 0.5 + rng->generateDouble()*0.5;
    else m_speed = Const::AST_MAX_SPEED + rng->generateDouble()*0.5;

    chooseSprite();
}

void Asteroid::chooseSprite() {
    auto* rng = QRandomGenerator::global();
    if (m_size == Large) m_variant = rng->bounded(kAsteroidSpritesLarge.size());
    else if (m_size == Medium) m_variant = rng->bounded(kAsteroidSpritesMedium.size());
    else m_variant = rng->bounded(kAsteroidSpritesSmall.size());
}

QRectF Asteroid::boundingRect() const {
    int dim = 0;
    if (m_size == Large) dim = 12 * Const::RASTER;
    else if (m_size == Medium) dim = 6 * Const::RASTER;
    else dim = 3 * Const::RASTER;
    return QRectF(-dim/2, -dim/2, dim, dim);
}

void Asteroid::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;
    
    
    const QVector<QString>& sprite = (m_size==Large) ? kAsteroidSpritesLarge[m_variant] :
                                     (m_size==Medium) ? kAsteroidSpritesMedium[m_variant] : kAsteroidSpritesSmall[m_variant];
    drawPixelArt(p, sprite, kAsteroidPalette, true);
}

void Asteroid::advance(int phase) {
    if (phase == 0) return;
    auto* g = Game::instance();
    if (!g) return;
    
    moveBy(0, m_speed * g->difficultySpeedMul());
    
    if (y() > Const::SCENE_H + 50) {
        g->asteroidLeaked(); 
        deleteLater();
        return;
    }

    
    const auto items = collidingItems();
    for (QGraphicsItem* item : items) {
        if (dynamic_cast<Player*>(item)) {
            
            
            
            g->playerHit(30);
            
            destroy(true); 
            return;
        }
    }
}

void Asteroid::hit() { destroy(true); }

void Asteroid::destroy(bool split) {
    auto* sc = scene();
    auto* g  = Game::instance();
    if (!sc) { deleteLater(); return; }

    auto* e = new Explosion();
    e->setPos(pos());
    sc->addItem(e);
    
    g->addScore(scoreValue());

    
    
    if (split && m_size == Large) {
        Size next = Medium;
        for (int i = 0; i < 2; ++i) {
            auto* a = new Asteroid(next);
            qreal offX = (i == 0 ? -20 : 20);
            a->setPos(x() + offX, y());
            sc->addItem(a);
        }
    } 
    
    
    if (g && m_size == Large) {
        if (QRandomGenerator::global()->bounded(100) < 10) {
            g->spawnPowerUp(pos());
        }
    }
    
    deleteLater();
}

int Asteroid::scoreValue() const {
    switch(m_size) {
        case Large: return Const::AST_SCORE_LARGE;
        case Medium: return Const::AST_SCORE_MED;
        case Small: return Const::AST_SCORE_SMALL;
    }
    return 0;
}