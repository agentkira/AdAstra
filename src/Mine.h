#pragma once
#include <QGraphicsObject>

class Mine : public QGraphicsObject {
    Q_OBJECT
public:
    Mine();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

    
    void explodeVigorously();

private:
    qreal m_sinePhase { 0.0 };
};
