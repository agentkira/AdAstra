#pragma once
#include <QGraphicsObject>
#include <QElapsedTimer>
#include <QKeyEvent>

class Player : public QGraphicsObject {
    Q_OBJECT
public:
    Player();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

    void setInvuln(int ms);
    
    
    void activateShield();
    void activateTriShot();
    bool hasShield() const { return m_hasShield; }
    void consumeShield(); 

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    bool m_leftHeld  = false;
    bool m_rightHeld = false;
    bool m_shootHeld = false;

    QElapsedTimer m_fireTimer;
    bool   m_invuln      = false;
    qint64 m_invulnEndMs = 0;
    qreal  m_moveAcc = 0.0;

    
    bool m_hasShield = false;
    bool m_hasTriShot = false;
    qint64 m_triShotEndMs = 0;

    void doShootIfReady();
    void stepMove();
};