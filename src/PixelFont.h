#pragma once
#include <QPainter>
#include <QMap>
#include <QVector>
#include <QString>
#include "Constants.h"

class PixelFont {
public:
    PixelFont(QColor color = Qt::white, QColor shadow = QColor(40,40,40,180))
        : m_color(color), m_shadow(shadow) {}

    void draw(QPainter* p, const QString& text, QPoint topLeft, int scale = 1) const;

private:
    QColor m_color, m_shadow;
    QVector<QString> glyph(char c) const;
};
