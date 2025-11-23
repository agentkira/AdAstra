#pragma once
#include <QGraphicsObject>

class Bullet : public QGraphicsObject {
    Q_OBJECT
public:
    Bullet();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

private:
    qreal m_accum = 0.0;
};
