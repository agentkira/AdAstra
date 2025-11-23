#pragma once
#include <QGraphicsObject>
#include <QPainter>

class PowerUp : public QGraphicsObject {
    Q_OBJECT
public:
    
    enum Type {
        Shield,
        TriShot,
        HealthPack
    };

    explicit PowerUp(Type type);

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

    Type getType() const { return m_type; }

private:
    Type m_type;
    qreal m_sinePhase { 0.0 };
};