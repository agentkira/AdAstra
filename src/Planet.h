#pragma once
#include <QGraphicsObject>
#include <QPixmap>

enum class BiomeType { Frost, Fire, Jungle, Dust, Ocean, Cyber };

class Planet : public QGraphicsObject {
    Q_OBJECT
public:
    explicit Planet(BiomeType type);
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;
    
    
    void accelerateExit(); 
    BiomeType biome() const { return m_type; }

private:
    BiomeType m_type;
    qreal m_speed;
    bool m_exiting = false;
    QPixmap m_sprite;
    void generateSprite();
};