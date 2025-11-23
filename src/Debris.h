#pragma once
#include <QGraphicsObject>

class Debris : public QGraphicsObject {
    Q_OBJECT
public:
    Debris();
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

private:
    int m_life    { 10 };  
    int m_maxLife { 10 };  
    int m_stepX   { 0 };   
    int m_stepY   { 0 };   
};
