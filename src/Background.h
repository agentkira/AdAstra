#pragma once
#include <QGraphicsObject>
#include <QColor>
#include "Planet.h"

class Background : public QGraphicsObject {
    Q_OBJECT
public:
    Background();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    void advance(int phase) override;

    void setRound(int round);

private:
    int m_currentRound { 1 };
    BiomeType m_currentBiome { BiomeType::Frost };
    BiomeType m_targetBiome  { BiomeType::Frost };

    
    QColor m_currentColor;
    QColor m_targetColor;
    qreal  m_colorProgress { 1.0 };

    QColor getSkyColor(BiomeType b) const;
    BiomeType getBiomeForRound(int r) const;
    void spawnPlanet();
};