#pragma once
#include <QGraphicsObject>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QImage>
#include <QList>

struct HighScoreEntry; 

class HUD : public QGraphicsObject {
    Q_OBJECT
public:
    HUD();

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    QImage renderPixelText(const QString &text, int fontSize, const QColor &color);

    void showMenuOverlay(const QString& playerName);
    void showPauseOverlay();
    void showGameOverOverlay(int finalScore);
    void showLeaderboard(const QList<HighScoreEntry>& scores);
    void showInstructions();
    void hideOverlays();

    void onHudChanged(int score, int hp, int lives);
    void updatePowerUpTimers(float shieldPct, float rapidPct); 

    void showRoundBanner(int round, const QString &quote);
    void triggerFlash();

private:
    void ensureRoundItems();
    void layoutRoundItems();

    
    int m_score { 0 };
    int m_hp    { 0 };
    int m_lives { 0 };
    
    
    float m_shieldPct { 0.0f };
    float m_rapidPct  { 0.0f };

    
    QGraphicsPixmapItem *m_menuText     { nullptr };
    QGraphicsPixmapItem *m_pauseText    { nullptr };
    QGraphicsPixmapItem *m_gameOverText { nullptr };
    QGraphicsPixmapItem *m_infoText     { nullptr }; 

    
    QGraphicsRectItem   *m_roundBack  { nullptr };
    QGraphicsPixmapItem *m_roundTitle { nullptr };
    QGraphicsPixmapItem *m_roundQuote { nullptr };
    QTimer m_roundFade;
    int m_roundFadeSteps { 0 };

    
    QTimer m_flashTimer;
    int m_flashAlpha { 0 };
};