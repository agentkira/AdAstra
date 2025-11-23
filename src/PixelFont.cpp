#include "PixelFont.h"

void PixelFont::draw(QPainter* p, const QString& text, QPoint topLeft, int scale) const {
    if (!p) return;
    p->save();
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);

    int xoff = 0;
    for (QChar qc : text) {
        char c = qc.toUpper().toLatin1();
        auto g = glyph(c);
        for (int y = 0; y < g.size(); ++y) {
            const QString& row = g[y];
            for (int x = 0; x < row.size(); ++x) {
                if (row[x] == '#') {
                    QRect r(topLeft.x() + (xoff + x) * Const::RASTER * scale,
                            topLeft.y() + y * Const::RASTER * scale,
                            Const::RASTER * scale,
                            Const::RASTER * scale);
                    p->fillRect(r.translated(Const::RASTER/4, Const::RASTER/4), m_shadow);
                    p->fillRect(r, m_color);
                }
            }
        }
        xoff += 6; 
    }
    p->restore();
}

QVector<QString> PixelFont::glyph(char c) const {
    static QMap<char, QVector<QString>> map = {
        { 'A', {"..#..",".#.#.","#####","#...#","#...#"} },
        { 'B', {"####.","#...#","####.","#...#","####."} },
        { 'C', {".####","#....","#....","#....",".####"} },
        { 'E', {"#####","#....","#####","#....","#####"} },
        { 'R', {"####.","#...#","####.","#..#.","#...#"} },
        { 'O', {".###.","#...#","#...#","#...#",".###."} },
        { 'U', {"#...#","#...#","#...#","#...#",".###."} },
        { 'N', {"#...#","##..#","#.#.#","#..##","#...#"} },
        { 'D', {"####.","#...#","#...#","#...#","####."} },
        { '1', {"..#..",".##..","..#..","..#..",".###."} },
        { '2', {".###.","#...#","...#.","..#..","#####"} },
        { '3', {"#####","...#.",".###.","...#.","#####"} },
        { ' ', {".....",".....",".....",".....","....."} }
    };
    return map.value(c, map.value(' '));
}
