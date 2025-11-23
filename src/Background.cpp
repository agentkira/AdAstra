#include "Background.h"
#include "Constants.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QGraphicsScene> 

Background::Background() {
    setZValue(Const::Z_BG_COLOR);
    m_currentBiome = BiomeType::Frost;
    m_targetBiome  = BiomeType::Frost;
    m_currentColor = getSkyColor(m_currentBiome);
    m_targetColor  = m_currentColor;
}

QRectF Background::boundingRect() const {
    return QRectF(0, 0, Const::SCENE_W, Const::SCENE_H);
}

void Background::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;
    p->fillRect(boundingRect(), m_currentColor);
}

void Background::advance(int phase) {
    if (phase == 0) return;

    
    if (m_colorProgress < 1.0) {
        m_colorProgress += 0.005; 
        if (m_colorProgress > 1.0) m_colorProgress = 1.0;

        
        int r = m_currentColor.red()   + (m_targetColor.red()   - m_currentColor.red())   * 0.02;
        int g = m_currentColor.green() + (m_targetColor.green() - m_currentColor.green()) * 0.02;
        int b = m_currentColor.blue()  + (m_targetColor.blue()  - m_currentColor.blue())  * 0.02;
        m_currentColor.setRgb(r, g, b);
        update(); 
    }

    
    auto* rng = QRandomGenerator::global();
    if (rng->bounded(Const::PLANET_SPAWN_CHANCE) == 0) {
        spawnPlanet();
    }
}

void Background::setRound(int round) {
    if (m_currentRound == round) return;
    m_currentRound = round;

    BiomeType next = getBiomeForRound(round);
    if (next != m_targetBiome) {
        m_targetBiome = next;
        m_targetColor = getSkyColor(next);
        m_colorProgress = 0.0; 
    }
}

BiomeType Background::getBiomeForRound(int r) const {
    
    int idx = (r - 1) % 6;
    return static_cast<BiomeType>(idx);
}

QColor Background::getSkyColor(BiomeType b) const {
    
    switch (b) {
        case BiomeType::Frost:  return QColor(10, 15, 35);   
        case BiomeType::Fire:   return QColor(30, 10, 10);   
        case BiomeType::Jungle: return QColor(10, 25, 15);   
        case BiomeType::Dust:   return QColor(25, 20, 15);   
        case BiomeType::Ocean:  return QColor(5, 10, 40);    
        case BiomeType::Cyber:  return QColor(20, 5, 30);    
        default: return Qt::black;
    }
}

void Background::spawnPlanet() {
    if (!scene()) return;
    
    BiomeType t = (m_colorProgress > 0.5) ? m_targetBiome : m_currentBiome;
    Planet* p = new Planet(t);
    scene()->addItem(p);
}