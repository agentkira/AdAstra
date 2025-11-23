#pragma once
#include <QGraphicsItem>

class Star : public QGraphicsItem {
public:
    enum class Layer { Back, Front };
    explicit Star(Layer l);

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

private:
    Layer m_layer;
    qreal m_speed;
    qreal m_accum = 0;
};
