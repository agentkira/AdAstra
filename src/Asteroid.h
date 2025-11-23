#pragma once
#include <QGraphicsObject>
#include <QElapsedTimer>

class Asteroid : public QGraphicsObject {
    Q_OBJECT
public:
    enum Size { Small, Medium, Large };

    explicit Asteroid(Size s = Large);

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

    void hit();               
    void destroy(bool split); 
    int  scoreValue() const;  

private:
    Size          m_size;
    qreal         m_speed {1.0};
    QElapsedTimer m_life;
    int           m_variant {0};

    void chooseSprite();
};
