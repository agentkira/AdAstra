#include "HUD.h"
#include "Constants.h"
#include "Game.h" 
#include <QPainter>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QFont>
#include <QBrush>
#include <QFontDatabase>
#include <QGraphicsPixmapItem>
#include <QTimer>

QImage HUD::renderPixelText(const QString &text, int fontSize, const QColor &color) {
    QFont f("Courier", fontSize);
    f.setStyleStrategy(QFont::NoAntialias);
    f.setBold(true);
    
    QFontMetrics fm(f);
    int w = fm.horizontalAdvance(text) + 4; 
    int h = fm.height() + 2;

    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QPainter p(&img);
    p.setFont(f);
    p.setPen(color);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setRenderHint(QPainter::TextAntialiasing, false);
    p.drawText(2, fm.ascent(), text);
    p.end();

    return img.scaled(img.width() * 2, img.height() * 2, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

HUD::HUD() {
    setZValue(Const::Z_HUD);

    
    m_menuText = new QGraphicsPixmapItem(this);
    m_menuText->setVisible(false);

    
    m_pauseText = new QGraphicsPixmapItem(this);
    QImage pauseImg = renderPixelText("PAUSED", 16, Qt::yellow);
    m_pauseText->setPixmap(QPixmap::fromImage(pauseImg));
    m_pauseText->setPos((Const::SCENE_W - pauseImg.width())/2, Const::SCENE_H/2);
    m_pauseText->setVisible(false);

    
    m_gameOverText = new QGraphicsPixmapItem(this);
    m_gameOverText->setVisible(false);

    
    m_infoText = new QGraphicsPixmapItem(this);
    m_infoText->setVisible(false);
    m_infoText->setZValue(1006);

    connect(&m_flashTimer, &QTimer::timeout, this, [this](){
        m_flashAlpha -= 20;
        if(m_flashAlpha <= 0) {
            m_flashAlpha = 0;
            m_flashTimer.stop();
        }
        update();
    });

    connect(&m_roundFade, &QTimer::timeout, this, [this](){
        if (m_roundFadeSteps > 0) {
            m_roundFadeSteps--;
            if (m_roundBack) {
                int alpha = qMin(220, m_roundFadeSteps * 5);
                m_roundBack->setBrush(QColor(0,0,0, alpha));
                if (m_roundTitle) m_roundTitle->setOpacity(qMin(1.0, m_roundFadeSteps * 0.05));
                if (m_roundQuote) m_roundQuote->setOpacity(qMin(1.0, m_roundFadeSteps * 0.05));
            }
            if (m_roundFadeSteps == 0) {
                if (m_roundBack) m_roundBack->setVisible(false);
                if (m_roundTitle) m_roundTitle->setVisible(false);
                if (m_roundQuote) m_roundQuote->setVisible(false);
                m_roundFade.stop();
            }
        }
    });
}

QRectF HUD::boundingRect() const {
    return QRectF(0, 0, Const::SCENE_W, Const::SCENE_H);
}

void HUD::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!p) return;

    p->setRenderHint(QPainter::Antialiasing, false);

    
    p->setPen(Qt::white);
    p->setFont(QFont("Courier", 14, QFont::Bold));
    p->drawText(20, 30, QString("SCORE: %1").arg(m_score, 6, 10, QChar('0')));

    
    int barW = 200;
    int barH = 16;
    int barX = Const::SCENE_W - barW - 20;
    int barY = 15;
    
    p->setPen(Qt::white);
    p->setBrush(Qt::NoBrush);
    p->drawRect(barX, barY, barW, barH);
    
    if (m_hp > 0) {
        QColor hpCol = Qt::green;
        if (m_hp < 30) hpCol = Qt::red;
        else if (m_hp < 60) hpCol = Qt::yellow;
        
        p->setPen(Qt::NoPen);
        p->setBrush(hpCol);
        int fillW = int(barW * (m_hp / (float)Const::PLAYER_MAX_HP));
        p->drawRect(barX+1, barY+1, fillW-1, barH-1);
    }

    
    p->setPen(Qt::white);
    p->drawText(barX, barY + 35, QString("LIVES: %1").arg(m_lives));

    
    if (m_shieldPct > 0) {
        p->setPen(Qt::white);
        p->setBrush(Qt::NoBrush);
        p->drawRect(20, 50, 100, 8);
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::cyan);
        p->drawRect(21, 51, int(98 * m_shieldPct), 6);
        
        
        QImage t = renderPixelText("SHIELD", 8, Qt::white);
        p->drawImage(130, 45, t);
    }

    if (m_rapidPct > 0) {
        int yOff = (m_shieldPct > 0) ? 70 : 50;
        p->setPen(Qt::white);
        p->setBrush(Qt::NoBrush);
        p->drawRect(20, yOff, 100, 8);
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::yellow);
        p->drawRect(21, yOff+1, int(98 * m_rapidPct), 6);
        
        
        QImage t = renderPixelText("RAPID", 8, Qt::white);
        p->drawImage(130, yOff-5, t);
    }

    
    if (m_flashAlpha > 0) {
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(255, 255, 255, m_flashAlpha));
        p->drawRect(boundingRect());
    }
}

void HUD::updatePowerUpTimers(float shieldPct, float rapidPct) {
    m_shieldPct = shieldPct;
    m_rapidPct = rapidPct;
    update();
}

void HUD::showMenuOverlay(const QString& playerName) {
    int w = 600; int h = 400;
    QImage pm(w, h, QImage::Format_ARGB32);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    
    QImage t1 = renderPixelText("AD ASTRA", 30, Qt::cyan);
    p.drawImage((w - t1.width())/2, 50, t1);
    
    QImage t2 = renderPixelText("PILOT: " + playerName, 12, Qt::yellow);
    p.drawImage((w - t2.width())/2, 150, t2);

    QImage t3 = renderPixelText("PRESS SPACE TO LAUNCH", 14, Qt::white);
    p.drawImage((w - t3.width())/2, 220, t3);
    
    QImage t4 = renderPixelText("[L] LEADERBOARD   [I] INSTRUCTIONS", 10, Qt::gray);
    p.drawImage((w - t4.width())/2, 300, t4);
    
    p.end();

    m_menuText->setPixmap(QPixmap::fromImage(pm));
    m_menuText->setPos((Const::SCENE_W - w)/2, 50);
    m_menuText->setVisible(true);
    m_pauseText->setVisible(false);
    m_gameOverText->setVisible(false);
    m_infoText->setVisible(false);
}

void HUD::showPauseOverlay() {
    m_menuText->setVisible(false);
    m_pauseText->setVisible(true);
    m_gameOverText->setVisible(false);
    m_infoText->setVisible(false);
}

void HUD::showLeaderboard(const QList<HighScoreEntry>& scores) {
    m_menuText->setVisible(false);
    int w = 500; int h = 500;
    QImage pm(w, h, QImage::Format_ARGB32);
    pm.fill(QColor(0,0,0,220));
    QPainter p(&pm);
    
    p.setPen(Qt::white);
    p.drawRect(0,0,w-1,h-1);
    
    QImage title = renderPixelText("HALL OF FAME", 20, Qt::yellow);
    p.drawImage((w - title.width())/2, 20, title);
    
    int y = 80;
    for (int i=0; i<scores.size(); ++i) {
        QString line = QString("%1. %2   %3").arg(i+1).arg(scores[i].name, -10).arg(scores[i].score);
        QImage row = renderPixelText(line, 12, (i==0 ? Qt::cyan : Qt::white));
        p.drawImage((w - row.width())/2, y, row);
        y += 35;
    }
    
    QImage footer = renderPixelText("PRESS ANY KEY", 10, Qt::gray);
    p.drawImage((w - footer.width())/2, h - 40, footer);
    
    p.end();
    
    m_infoText->setPixmap(QPixmap::fromImage(pm));
    m_infoText->setPos((Const::SCENE_W - w)/2, (Const::SCENE_H - h)/2);
    m_infoText->setVisible(true);
}

void HUD::showInstructions() {
    m_menuText->setVisible(false);
    int w = 600; int h = 400;
    QImage pm(w, h, QImage::Format_ARGB32);
    pm.fill(QColor(0,0,0,220));
    QPainter p(&pm);
    p.setPen(Qt::white);
    p.drawRect(0,0,w-1,h-1);

    int y = 30;
    auto drawLine = [&](QString t, int s, QColor c) {
        QImage im = renderPixelText(t, s, c);
        p.drawImage((w - im.width())/2, y, im);
        y += im.height() + 15;
    };

    drawLine("MISSION BRIEFING", 18, Qt::green);
    y += 10;
    drawLine("ARROWS / WASD : MOVE SHIP", 12, Qt::white);
    drawLine("SPACE : SHOOT", 12, Qt::white);
    drawLine("P : PAUSE GAME", 12, Qt::white);
    drawLine("AVOID ASTEROIDS AND MINES", 12, Qt::red);
    drawLine("COLLECT POWERUPS", 12, Qt::yellow);
    y += 20;
    drawLine("PRESS ANY KEY TO RETURN", 10, Qt::gray);

    p.end();
    
    m_infoText->setPixmap(QPixmap::fromImage(pm));
    m_infoText->setPos((Const::SCENE_W - w)/2, (Const::SCENE_H - h)/2);
    m_infoText->setVisible(true);
}

void HUD::showGameOverOverlay(int finalScore) {
    m_menuText->setVisible(false);
    m_pauseText->setVisible(false);
    m_infoText->setVisible(false);
    
    int w = 400; int h = 250;
    QImage pm(w, h, QImage::Format_ARGB32);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    
    QImage t1 = renderPixelText("GAME OVER", 20, Qt::red);
    QImage t2 = renderPixelText(QString("FINAL SCORE: %1").arg(finalScore), 12, Qt::white);
    QImage t3 = renderPixelText("PRESS SPACE TO RESTART", 10, Qt::white);
    QImage t4 = renderPixelText("PRESS ESC FOR MENU", 10, Qt::gray);
    
    int y = 0;
    p.drawImage((w - t1.width())/2, y, t1); y += t1.height() + 20;
    p.drawImage((w - t2.width())/2, y, t2); y += t2.height() + 30;
    p.drawImage((w - t3.width())/2, y, t3); y += t3.height() + 10;
    p.drawImage((w - t4.width())/2, y, t4);

    m_gameOverText->setPixmap(QPixmap::fromImage(pm));
    m_gameOverText->setPos((Const::SCENE_W - w)/2, Const::SCENE_H/3);
    m_gameOverText->setZValue(1005);
    m_gameOverText->setVisible(true);
}

void HUD::hideOverlays() {
    m_menuText->setVisible(false);
    m_pauseText->setVisible(false);
    m_gameOverText->setVisible(false);
    m_infoText->setVisible(false);
}

void HUD::onHudChanged(int score, int hp, int lives) {
    m_score = score;
    m_hp = hp;
    m_lives = lives;
    update();
}

void HUD::ensureRoundItems() {
    if (!m_roundBack) {
        m_roundBack = new QGraphicsRectItem(this);
        m_roundBack->setBrush(QColor(0,0,0,220));
        m_roundBack->setZValue(1001);

        m_roundTitle = new QGraphicsPixmapItem(this);
        m_roundTitle->setZValue(1002);

        m_roundQuote = new QGraphicsPixmapItem(this);
        m_roundQuote->setZValue(1002);
    }
}

void HUD::layoutRoundItems() {
    int w = qMax(m_roundTitle->pixmap().width(), m_roundQuote->pixmap().width()) + 60;
    int h = m_roundTitle->pixmap().height() + m_roundQuote->pixmap().height() + 40;
    int x = (Const::SCENE_W - w)/2;
    int y = Const::SCENE_H/3;
    
    m_roundBack->setRect(x, y, w, h);
    
    int ty = y + 20;
    m_roundTitle->setPos(x + (w - m_roundTitle->pixmap().width())/2, ty);
    ty += m_roundTitle->pixmap().height() + 10;
    m_roundQuote->setPos(x + (w - m_roundQuote->pixmap().width())/2, ty);
}

void HUD::showRoundBanner(int round, const QString &quote) {
    ensureRoundItems();
    
    QImage t1 = renderPixelText(QString("ROUND %1").arg(round), 16, Qt::cyan);
    m_roundTitle->setPixmap(QPixmap::fromImage(t1));
    
    QImage t2 = renderPixelText(quote, 10, Qt::white);
    m_roundQuote->setPixmap(QPixmap::fromImage(t2));
    
    layoutRoundItems();
    
    m_roundBack->setVisible(true);
    m_roundTitle->setVisible(true);
    m_roundQuote->setVisible(true);
    
    m_roundFadeSteps = 100; 
    m_roundFade.start(30);
}

void HUD::triggerFlash() {
    m_flashAlpha = 200;
    m_flashTimer.start(20);
}