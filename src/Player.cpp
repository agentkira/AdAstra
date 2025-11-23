#include "Player.h"
#include "Constants.h"
#include "PixelArt.h"
#include "Game.h"
#include "Bullet.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QtMath>
#include <QDateTime>

namespace {
const QVector<QString> kPlayerSprite = {
    ".......H.......",
    ".......H.......",
    ".......W.......",
    "......WGW......",
    "......WGW......",
    ".....WWGWW.....",
    ".....WGDGW.....",
    "....WWGDGWW....",
    "....WGGDGGW....",
    "...WGGGDGGGW...",
    "...WGGGDGGGW...",
    "..WWGGGDGGGWW..",
    ".WWGGGGGGGGGWW.",
    "WWRGGGGGGGGGRWW",
    "WWRGGGGGGGGGRWW",
    "WWRGGGGGGGGGRWW",
    "WWRR.......RRWW",
    "FFFF.......FFFF"
};

const QHash<QChar, QColor> kPlayerPalette = {
    { 'H', QColor(200, 200, 200) }, 
    { 'W', QColor(160, 170, 180) }, 
    { 'G', QColor(80, 90, 100) },  
    { 'D', QColor(40, 40, 50) },    
    { 'R', QColor(200, 50, 50) },   
    { 'F', QColor(255, 180, 0) }    
};
}

Player::Player() {
    setZValue(Const::Z_PLAYER);
    setFlag(QGraphicsItem::ItemIsFocusable);
}

QRectF Player::boundingRect() const {
    return QRectF(-30, -36, 60, 72);
}

void Player::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;

    if (m_invuln) {
        if ((m_fireTimer.elapsed() / 100) % 2 == 0) return;
    }

    drawPixelArt(p, kPlayerSprite, kPlayerPalette, true);

    
    if (m_hasShield) {
        p->save();
        QColor shieldCol(0, 200, 255);
        
        
        p->setPen(Qt::NoPen);
        p->setBrush(shieldCol);

        int cornerLen = 3 * Const::RASTER;
        int thick     = 1 * Const::RASTER;
        QRectF r      = boundingRect().adjusted(-thick, -thick, thick, thick);

        
        auto drawRect = [&](qreal x, qreal y, qreal w, qreal h) {
            p->drawRect(QRectF(x, y, w, h));
        };

        
        drawRect(r.left(), r.top(), cornerLen, thick);         
        drawRect(r.left(), r.top(), thick, cornerLen);         

        
        drawRect(r.right() - cornerLen, r.top(), cornerLen, thick);
        drawRect(r.right() - thick, r.top(), thick, cornerLen);

        
        drawRect(r.left(), r.bottom() - thick, cornerLen, thick);
        drawRect(r.left(), r.bottom() - cornerLen, thick, cornerLen);

        
        drawRect(r.right() - cornerLen, r.bottom() - thick, cornerLen, thick);
        drawRect(r.right() - thick, r.bottom() - cornerLen, thick, cornerLen);

        
        p->setBrush(QColor(0, 200, 255, 40));
        p->drawRect(r);
        
        p->restore();
    }
}

void Player::advance(int phase) {
    if (phase == 0) return;

    if (m_invuln && QDateTime::currentMSecsSinceEpoch() > m_invulnEndMs) {
        m_invuln = false;
    }

    Game* g = Game::instance();
    if (g) {
        m_hasTriShot = g->isRapidFireActive();
        m_hasShield  = g->isShieldActive();
    }

    stepMove();
    if (m_shootHeld) doShootIfReady();
}

void Player::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) return;
    switch (event->key()) {
    case Qt::Key_Left:  case Qt::Key_A: m_leftHeld = true; break;
    case Qt::Key_Right: case Qt::Key_D: m_rightHeld = true; break;
    case Qt::Key_Space: m_shootHeld = true; break;
    default: QGraphicsObject::keyPressEvent(event); break;
    }
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) return;
    switch (event->key()) {
    case Qt::Key_Left:  case Qt::Key_A: m_leftHeld = false; break;
    case Qt::Key_Right: case Qt::Key_D: m_rightHeld = false; break;
    case Qt::Key_Space: m_shootHeld = false; break;
    default: QGraphicsObject::keyReleaseEvent(event); break;
    }
}

void Player::doShootIfReady() {
    if (!m_fireTimer.isValid()) m_fireTimer.start();
    if (m_fireTimer.elapsed() < Const::PLAYER_COOLDOWN_MS) return;

    m_fireTimer.restart();
    auto* sc = scene();
    if (!sc) return;

    auto* b = new Bullet();
    b->setPos(x(), y() - 36);
    sc->addItem(b);

    if (m_hasTriShot) {
        Bullet* b1 = new Bullet();
        b1->setPos(x() - 10, y() - 20);
        sc->addItem(b1);

        Bullet* b2 = new Bullet();
        b2->setPos(x() + 10, y() - 20);
        sc->addItem(b2);
    }
}

void Player::stepMove() {
    if (m_leftHeld == m_rightHeld) return;
    const qreal dir  = m_leftHeld ? -1.0 : 1.0;
    const qreal step = Const::RASTER * 2.0; 

    double nextX = x() + dir * step;
    if (nextX < 30) nextX = 30;
    if (nextX > Const::SCENE_W - 30) nextX = Const::SCENE_W - 30;
    
    setX(nextX);
}

void Player::setInvuln(int ms) {
    m_invuln = true;
    m_invulnEndMs = QDateTime::currentMSecsSinceEpoch() + ms;
}

void Player::activateShield() {
    m_hasShield = true;
}

void Player::activateTriShot() {
    m_hasTriShot = true;
    m_triShotEndMs = m_fireTimer.elapsed() + Const::POWERUP_RAPID_FIRE_DURATION_MS;
}

void Player::consumeShield() {
    m_hasShield = false;
}