#pragma once
#include <QPainter>
#include <QVector>
#include <QString>
#include <QHash>
#include "Constants.h"

inline void drawPixelArt(QPainter* p,
                         const QVector<QString>& rows,
                         const QHash<QChar, QColor>& palette,
                         bool centered = true)
{
    if (!p || rows.isEmpty()) return;

    p->save();
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->scale(Const::RASTER, Const::RASTER);

    const int h = rows.size();
    const int w = rows[0].size();
    const qreal ox = centered ? -w / 2.0 : 0.0;
    const qreal oy = centered ? -h / 2.0 : 0.0;

    for (int y = 0; y < h; ++y) {
        const QString& row = rows[y];
        for (int x = 0; x < row.size(); ++x) {
            const QChar ch = row[x];
            if (ch == '.') continue;
            const auto it = palette.constFind(ch);
            if (it == palette.cend()) continue;
            p->fillRect(QRectF(ox + x, oy + y, 1, 1), it.value());
        }
    }
    p->restore();
}

inline qreal snapRaster(qreal v) {
    return std::round(v / Const::RASTER) * Const::RASTER;
}
