#pragma once
#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QElapsedTimer>
#include <QList>
#include <QPair>
#include <QSettings> 
#include "PowerUp.h" 

class Player;
class HUD;
class Background;

struct HighScoreEntry {
    QString name;
    int score;
    
    bool operator>(const HighScoreEntry& other) const {
        return score > other.score;
    }
};

class Game : public QObject {
    Q_OBJECT
public:
    static Game* instance();
    explicit Game(QObject* parent=nullptr);
    ~Game();

    
    void setPlayerName(const QString& name);

    void start();       
    void toMenu();
    
    
    void togglePause();
    void toggleGrid();
    void showLeaderboard();
    void showInstructions();
    void handleInput(int key); 

    bool m_showGrid = true;

    
    void addScore(int s);
    void playerHit(int dmg); 
    void asteroidLeaked();   
    void loseLife();         
    void gameOver();         
    void spawnPowerUp(QPointF pos);
    
    void shakeScene(int ms, qreal intensity);
    
    
    void activatePowerUp(PowerUp::Type type);
    bool isRapidFireActive() const;
    bool isShieldActive() const;

    qreal difficultySpeedMul() const;
    int   leakDamage() const; 
    
    Player* player() const { return m_player; }
    HUD* hud() const { return m_hud; }

    
    void saveHighScore(int score);
    QList<HighScoreEntry> getHighScores() const;

    
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void tick();
    void spawnAsteroid();
    void spawnMine();
    void spawnPowerUpTimer();

private:
    static Game* s_inst;
    QGraphicsScene* m_scene = nullptr;
    QGraphicsView* m_view  = nullptr;
    Player* m_player = nullptr;
    HUD* m_hud    = nullptr;
    Background* m_bg = nullptr;

    QTimer m_tick;
    QTimer m_astSpawn;
    QTimer m_mineSpawn;
    QTimer m_powerUpSpawn;

    
    enum class State { Menu, Playing, Paused, GameOver, Leaderboard, Instructions };
    State m_state = State::Menu;
    
    QString m_playerName = "PILOT";
    
    int m_score = 0;
    int m_hp    = 100;
    int m_lives = 3;
    int m_round = 1;

    QElapsedTimer m_gameTimer;
    
    
    qint64 m_rapidFireEndMs = 0;
    qint64 m_shieldEndMs    = 0;

    int m_astSpawnMs  = 0;
    int m_mineSpawnMs = 0;
    int m_powerUpSpawnMs = 0;
    int m_diffAccumMs = 0;
    
    
    int m_maxAsteroids = 5;
    int m_maxMines     = 2;

    int   m_shakeRemainMs  = 0;
    qreal m_shakeIntensity = 0.0;

    void updateShake();
    void loadHighScores();
    void writeHighScores();
    
    QList<HighScoreEntry> m_highScores;
};