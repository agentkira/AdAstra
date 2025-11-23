#pragma once
#include <QtGlobal>
#include <QString>

namespace Const {


inline constexpr int GRID_W = 28; 
inline constexpr int GRID_H = 18;
inline constexpr int TILE   = 32;
inline constexpr int SCENE_W = GRID_W * TILE;
inline constexpr int SCENE_H = GRID_H * TILE;
inline constexpr int RASTER = 4; 


inline constexpr int Z_BG_COLOR   = -100;
inline constexpr int Z_PLANETS    = -50;
inline constexpr int Z_STARS      = -20;
inline constexpr int Z_GRID       = -10;
inline constexpr int Z_ENTITIES   = 0;
inline constexpr int Z_POWERUPS   = 3;
inline constexpr int Z_PLAYER     = 5;
inline constexpr int Z_HUD        = 1000;


inline constexpr int FPS = 60;
inline constexpr int TICK_MS = 1000 / FPS;


inline constexpr int PLAYER_Y = SCENE_H - 2 * TILE;
inline constexpr int PLAYER_COOLDOWN_MS = 150; 
inline constexpr int PLAYER_MAX_HP = 100;
inline constexpr int PLAYER_LIVES = 3; 
inline constexpr int PLAYER_INVULN_MS = 2000; 


inline constexpr int POWERUP_RAPID_FIRE_DURATION_MS = 8000;
inline constexpr int POWERUP_SHIELD_DURATION_MS     = 8000; 
inline constexpr int MIN_POWERUP_SPAWN_MS           = 8000; 

inline constexpr int POWERUP_SPAWN_MS               = 12000; 
inline constexpr qreal POWERUP_SPEED                = 2.0;   


inline constexpr qreal BULLET_SPEED = 14.0;
inline constexpr int   BULLET_W_CELLS = 1;
inline constexpr int   BULLET_H_CELLS = 3;


inline constexpr qreal AST_MIN_SPEED = 0.5;  
inline constexpr qreal AST_MAX_SPEED = 1.5; 
inline constexpr int   AST_LEAK_DMG_DEFAULT = 0; 
inline constexpr int   AST_SCORE_SMALL = 30;
inline constexpr int   AST_SCORE_MED   = 20;
inline constexpr int   AST_SCORE_LARGE = 10;


inline constexpr qreal MINE_SPEED = 0.7; 
inline constexpr qreal MINE_SINE_AMPL = 12.0;
inline constexpr qreal MINE_SINE_FREQ = 0.03;


inline constexpr qreal PLANET_SPEED_MIN = 0.1;
inline constexpr qreal PLANET_SPEED_MAX = 0.4;
inline constexpr int   PLANET_SPAWN_CHANCE = 350; 


inline constexpr int EXP_FRAMES = 14;
inline constexpr int EXP_DEBRIS = 12;
inline constexpr int SHAKE_MS = 100;
inline constexpr qreal SHAKE_INTENSITY = 6.0;


inline constexpr int INIT_AST_SPAWN_MS  = 2800; 
inline constexpr int INIT_MINE_SPAWN_MS = 6000; 

inline constexpr int DIFF_STEP_PERIOD_MS = 15000; 

}