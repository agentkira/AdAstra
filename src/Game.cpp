#include "Game.h"
#include "Constants.h"
#include "Player.h"
#include "Asteroid.h"
#include "Mine.h"
#include "HUD.h"
#include "Star.h"
#include "Background.h"
#include "RasterGrid.h"
#include "PowerUp.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Debris.h"
#include "Planet.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRandomGenerator>
#include <QShortcut>
#include <QTimer>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>

Game* Game::s_inst = nullptr;

static const QVector<QString> kRoundQuotes = {
    "FROSTY FRONTIER", "INTO THE FIRE", "JUNGLE ORBIT", 
    "DUST & SHADOWS", "ABYSSAL GAZE", "CYBERNETIC HORIZON", "BEYOND THE STARS"
};

static const QVector<int> kRoundScore = {
    0, 2000, 4500, 7500, 11000, 15000, 20000, 30000
};

Game::Game(QObject *parent) : QObject(parent) {
    s_inst = this;
    m_scene = new QGraphicsScene(0, 0, Const::SCENE_W, Const::SCENE_H, this);
    m_scene->setBackgroundBrush(Qt::black);

    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing, false);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_view->setRenderHint(QPainter::TextAntialiasing, false);
    m_view->setFixedSize(Const::SCENE_W, Const::SCENE_H);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setWindowTitle("Ad Astra - Retro Shooter");
    
    m_view->installEventFilter(this);
    m_view->show();

    loadHighScores();

    connect(&m_tick, &QTimer::timeout, this, &Game::tick);
    m_tick.setInterval(Const::TICK_MS);

    connect(&m_astSpawn, &QTimer::timeout, this, &Game::spawnAsteroid);
    connect(&m_mineSpawn, &QTimer::timeout, this, &Game::spawnMine);
    connect(&m_powerUpSpawn, &QTimer::timeout, this, &Game::spawnPowerUpTimer);

    toMenu();
}

Game::~Game() {
    s_inst = nullptr;
    delete m_view; 
}

Game* Game::instance() { return s_inst; }

void Game::setPlayerName(const QString &name) {
    m_playerName = name.trimmed().toUpper();
    if(m_playerName.isEmpty()) m_playerName = "UNKNOWN";
    if(m_state == State::Menu && m_hud) {
        m_hud->showMenuOverlay(m_playerName);
    }
}

bool Game::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        handleInput(ke->key());
        return false; 
    }
    return QObject::eventFilter(watched, event);
}

void Game::handleInput(int key) {
    if (key == Qt::Key_G) toggleGrid();

    switch (m_state) {
    case State::Menu:
        if (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return) {
            start();
        } else if (key == Qt::Key_L) {
            showLeaderboard();
        } else if (key == Qt::Key_I) {
            showInstructions();
        }
        break;

    case State::Playing:
        if (key == Qt::Key_P) togglePause();
        break;

    case State::Paused:
        if (key == Qt::Key_P) togglePause();
        break;

    case State::GameOver:
        if (key == Qt::Key_Space || key == Qt::Key_Enter) {
            start();
        } else if (key == Qt::Key_Escape) {
            toMenu();
        }
        break;

    case State::Leaderboard:
    case State::Instructions:
        toMenu();
        break;
    }
}

void Game::start() {
    m_scene->clear();
    m_score = 0;
    m_hp    = Const::PLAYER_MAX_HP;
    m_lives = Const::PLAYER_LIVES;
    m_round = 1;
    m_state = State::Playing;
    m_diffAccumMs = 0;
    m_rapidFireEndMs = 0;
    m_shieldEndMs = 0;
    m_gameTimer.start();

    m_astSpawnMs  = Const::INIT_AST_SPAWN_MS;
    m_mineSpawnMs = Const::INIT_MINE_SPAWN_MS;
    m_powerUpSpawnMs = Const::POWERUP_SPAWN_MS; 

    m_bg = new Background();
    m_scene->addItem(m_bg);

    for (int i=0; i<40; ++i) m_scene->addItem(new Star(Star::Layer::Back));
    for (int i=0; i<20; ++i) m_scene->addItem(new Star(Star::Layer::Front));

    RasterGrid* grid = new RasterGrid();
    grid->setVisible(m_showGrid);
    m_scene->addItem(grid);

    m_player = new Player();
    m_player->setPos(Const::SCENE_W/2, Const::PLAYER_Y);
    m_scene->addItem(m_player);
    m_player->setFocus();

    m_hud = new HUD();
    m_scene->addItem(m_hud);
    m_hud->onHudChanged(m_score, m_hp, m_lives);
    m_hud->showRoundBanner(m_round, kRoundQuotes[0]);

    m_tick.start();
    m_astSpawn.start(m_astSpawnMs);
    m_mineSpawn.start(m_mineSpawnMs);
    m_powerUpSpawn.start(m_powerUpSpawnMs);
}

void Game::toMenu() {
    m_state = State::Menu;
    m_tick.stop();
    m_astSpawn.stop();
    m_mineSpawn.stop();
    m_powerUpSpawn.stop();
    
    m_scene->clear();
    m_bg = nullptr; 
    
    m_bg = new Background(); 
    m_scene->addItem(m_bg);
    for (int i=0; i<40; ++i) m_scene->addItem(new Star(Star::Layer::Back));

    m_hud = new HUD();
    m_scene->addItem(m_hud);
    m_hud->showMenuOverlay(m_playerName);
}

void Game::showLeaderboard() {
    m_state = State::Leaderboard;
    if(m_hud) m_hud->showLeaderboard(m_highScores);
}

void Game::showInstructions() {
    m_state = State::Instructions;
    if(m_hud) m_hud->showInstructions();
}

void Game::tick() {
    if (m_state != State::Playing) return;

    m_scene->advance();
    updateShake();

    m_diffAccumMs += Const::TICK_MS;
    if (m_diffAccumMs > Const::DIFF_STEP_PERIOD_MS) {
        m_diffAccumMs = 0;
        m_astSpawnMs = qMax(300, int(m_astSpawnMs * 0.98));
        m_mineSpawnMs= qMax(800, int(m_mineSpawnMs * 0.98));
        m_astSpawn.setInterval(m_astSpawnMs);
        m_mineSpawn.setInterval(m_mineSpawnMs);
    }

    if (m_round < kRoundScore.size() && m_score >= kRoundScore[m_round]) {
        m_round++;
        if (m_bg) m_bg->setRound(m_round);
        QString q = (m_round <= kRoundQuotes.size()) ? kRoundQuotes[m_round-1] : "INFINITE VOID";
        m_hud->showRoundBanner(m_round, q);
        m_hp = qMin(Const::PLAYER_MAX_HP, m_hp + 20);
        m_hud->onHudChanged(m_score, m_hp, m_lives);
    }

    qint64 now = m_gameTimer.elapsed();
    float shieldPct = 0.0f;
    float rapidPct = 0.0f;

    if (m_shieldEndMs > now) {
        float total = Const::POWERUP_SHIELD_DURATION_MS;
        float left = m_shieldEndMs - now;
        shieldPct = left / total;
    } else {
        m_shieldEndMs = 0;
    }

    if (m_rapidFireEndMs > now) {
        float total = Const::POWERUP_RAPID_FIRE_DURATION_MS;
        float left = m_rapidFireEndMs - now;
        rapidPct = left / total;
    } else {
        m_rapidFireEndMs = 0;
    }

    if (m_hud) m_hud->updatePowerUpTimers(shieldPct, rapidPct);
}

void Game::spawnAsteroid() {
    if (m_state != State::Playing) return;
    int count = 0;
    for(auto* i : m_scene->items()) if(dynamic_cast<Asteroid*>(i)) count++;
    if(count >= m_maxAsteroids + m_round) return;

    Asteroid* a = new Asteroid();
    int x = QRandomGenerator::global()->bounded(Const::SCENE_W - 40) + 20;
    a->setPos(x, -50);
    m_scene->addItem(a);
}

void Game::spawnMine() {
    if (m_state != State::Playing) return;
    if (m_round < 2) return;

    int count = 0;
    for(auto* i : m_scene->items()) if(dynamic_cast<Mine*>(i)) count++;
    if(count >= m_maxMines + (m_round/2)) return;

    Mine* m = new Mine();
    int x = QRandomGenerator::global()->bounded(Const::SCENE_W - 40) + 20;
    m->setPos(x, -50);
    m_scene->addItem(m);
}

void Game::spawnPowerUp(QPointF pos) {
    int r = QRandomGenerator::global()->bounded(100);
    PowerUp::Type t = PowerUp::Shield;
    if (r < 40) t = PowerUp::TriShot;
    else if (r < 70) t = PowerUp::HealthPack; 
    auto* p = new PowerUp(t);
    p->setPos(pos);
    m_scene->addItem(p);
}

void Game::spawnPowerUpTimer() {
    if (m_state != State::Playing) return;
    int r = QRandomGenerator::global()->bounded(100);
    
    
    PowerUp::Type t = PowerUp::TriShot;
    if (r < 33) t = PowerUp::Shield;
    else if (r < 66) t = PowerUp::HealthPack;
    
    int x = QRandomGenerator::global()->bounded(Const::SCENE_W - 40) + 20;
    
    auto* p = new PowerUp(t);
    p->setPos(x, -50);
    m_scene->addItem(p);
}

void Game::activatePowerUp(PowerUp::Type type) {
    qint64 now = m_gameTimer.elapsed();

    switch (type) {
    case PowerUp::Shield:
        m_shieldEndMs = now + Const::POWERUP_SHIELD_DURATION_MS;
        if (m_player) m_player->activateShield();
        break;
    case PowerUp::TriShot:
        m_rapidFireEndMs = now + Const::POWERUP_RAPID_FIRE_DURATION_MS;
        if (m_player) m_player->activateTriShot();
        break;
    case PowerUp::HealthPack:
        m_hp = qMin(Const::PLAYER_MAX_HP, m_hp + 30);
        if (m_hud) m_hud->onHudChanged(m_score, m_hp, m_lives);
        break;
    }
}

bool Game::isRapidFireActive() const {
    return (m_rapidFireEndMs > 0 && m_gameTimer.elapsed() < m_rapidFireEndMs);
}

bool Game::isShieldActive() const {
    return (m_shieldEndMs > 0 && m_gameTimer.elapsed() < m_shieldEndMs);
}

void Game::addScore(int s) {
    m_score += s;
    if (m_hud) m_hud->onHudChanged(m_score, m_hp, m_lives);
}

void Game::asteroidLeaked() {
    playerHit(leakDamage());
}

void Game::playerHit(int dmg) {
    if (m_state != State::Playing) return;
    if (isShieldActive()) return; 

    m_hp -= dmg;
    if (m_hp <= 0) {
        m_hp = 0;
        loseLife();
    } else {
        if (m_player) m_player->setInvuln(1000);
        shakeScene(200, 5.0);
    }
    if (m_hud) m_hud->onHudChanged(m_score, m_hp, m_lives);
}

void Game::loseLife() {
    m_lives--;
    m_hp = Const::PLAYER_MAX_HP;
    
    if (m_hud) m_hud->onHudChanged(m_score, m_hp, m_lives);

    if (m_lives <= 0) {
        gameOver();
    } else {
        if (m_player) {
            m_player->setPos(Const::SCENE_W/2, Const::PLAYER_Y);
            m_player->setInvuln(Const::PLAYER_INVULN_MS);
        }
        shakeScene(500, 10.0);
    }
}

void Game::gameOver() {
    m_state = State::GameOver;
    m_astSpawn.stop();
    m_mineSpawn.stop();
    m_powerUpSpawn.stop();
    
    if (m_player) {
        Explosion* ex = new Explosion();
        ex->setPos(m_player->pos());
        m_scene->addItem(ex);
        m_player->setVisible(false);
    }

    saveHighScore(m_score);
    if (m_hud) m_hud->showGameOverOverlay(m_score);
}

qreal Game::difficultySpeedMul() const {
    return 1.0 + (m_round * 0.1); 
}

int Game::leakDamage() const { return 10; }

void Game::shakeScene(int ms, qreal intensity) {
    m_shakeRemainMs = ms;
    m_shakeIntensity = intensity;
}

void Game::updateShake() {
    if (m_shakeRemainMs <= 0) {
        m_view->setTransform(QTransform());
        return;
    }
    m_shakeRemainMs -= Const::TICK_MS;
    qreal dx = (QRandomGenerator::global()->generateDouble() - 0.5) * m_shakeIntensity;
    qreal dy = (QRandomGenerator::global()->generateDouble() - 0.5) * m_shakeIntensity;
    QTransform t;
    t.translate(dx, dy);
    m_view->setTransform(t);
}

void Game::togglePause() {
    if (m_state == State::Playing) {
        m_state = State::Paused;
        m_tick.stop();
        m_astSpawn.stop();
        m_mineSpawn.stop();
        if(m_hud) m_hud->showPauseOverlay();
    } else if (m_state == State::Paused) {
        m_state = State::Playing;
        m_tick.start();
        m_astSpawn.start();
        m_mineSpawn.start();
        if(m_hud) m_hud->hideOverlays();
    }
}

void Game::toggleGrid() {
    m_showGrid = !m_showGrid;
    for (auto* item : m_scene->items()) {
        if (dynamic_cast<RasterGrid*>(item)) {
            item->setVisible(m_showGrid);
        }
    }
}



void Game::saveHighScore(int score) {
    if (score <= 0) return;
    HighScoreEntry e;
    e.name = m_playerName;
    e.score = score;
    m_highScores.append(e);
    
    std::sort(m_highScores.begin(), m_highScores.end(), std::greater<HighScoreEntry>());
    while(m_highScores.size() > 10) m_highScores.removeLast();
    
    writeHighScores();
}

QList<HighScoreEntry> Game::getHighScores() const {
    return m_highScores;
}

void Game::loadHighScores() {
    QFile f("scores.json");
    if (!f.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    QJsonArray arr = doc.array();
    m_highScores.clear();
    
    for (auto val : arr) {
        QJsonObject obj = val.toObject();
        HighScoreEntry e;
        e.name = obj["name"].toString();
        e.score = obj["score"].toInt();
        m_highScores.append(e);
    }
}

void Game::writeHighScores() {
    QJsonArray arr;
    for (const auto& e : m_highScores) {
        QJsonObject obj;
        obj["name"] = e.name;
        obj["score"] = e.score;
        arr.append(obj);
    }
    QFile f("scores.json");
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(arr).toJson());
    }
}