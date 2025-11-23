#include "Bullet.h"
#include "Constants.h"
#include "PixelArt.h"
#include "Asteroid.h"
#include "Mine.h"
#include "Game.h"
#include "Explosion.h"
#include "HUD.h"        

#include <QGraphicsScene>
#include <QPainter>




namespace {

const QVector<QString> kBulletSprite = {
    "..yy..",
    ".yyyy.",
    ".yyyy.",
    "..yy.."
};

const QHash<QChar, QColor> kBulletPalette = {
    { 'y', QColor(250, 240, 180) }    
};

} 





Bullet::Bullet() {
    setZValue(4);
}





QRectF Bullet::boundingRect() const {
    const int w = kBulletSprite[0].size() * Const::RASTER;
    const int h = kBulletSprite.size()   * Const::RASTER;
    return QRectF(-w / 2.0, -h / 2.0, w, h);
}





void Bullet::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    drawPixelArt(p, kBulletSprite, kBulletPalette, true);
}





void Bullet::advance(int phase) {
    if (phase == 0) return;

    
    m_accum += Const::BULLET_SPEED;
    while (m_accum >= Const::RASTER) {
        moveBy(0, -Const::RASTER);
        m_accum -= Const::RASTER;
    }

    
    if (y() + boundingRect().bottom() < 0) {
        deleteLater();
        return;
    }

    Game* g = Game::instance();
    const auto items = collidingItems();

    for (QGraphicsItem* it : items) {

        
        
        
        if (auto* a = dynamic_cast<Asteroid*>(it)) {
            a->hit();
            deleteLater();
            return;
        }

        
        
        
        if (auto* m = dynamic_cast<Mine*>(it)) {
            if (g) {
                
                g->playerHit(Const::PLAYER_MAX_HP);

                
                g->shakeScene(
                    int(Const::SHAKE_MS * 2.0),
                    Const::SHAKE_INTENSITY * 1.6
                    );

                
                if (auto* hud = g->hud()) {
                    hud->triggerFlash();
                }
            }

            
            m->explodeVigorously();

            deleteLater();
            return;
        }
    }
}


