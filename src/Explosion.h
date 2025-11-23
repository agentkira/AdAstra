#pragma once
#include <QGraphicsObject>

class Explosion : public QGraphicsObject {
    Q_OBJECT
public:
    Explosion();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

private:
    int  m_frame {0};
    bool m_debrisSpawned {false};
};
