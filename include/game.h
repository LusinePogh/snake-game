#pragma once
#include "pos.h"
#include <vector>
#include "food.h"
#include <string>

enum class Dir { UP, DOWN, LEFT, RIGHT };

enum class GameState {
    MENU,       // Main menu with Start/Exit
    PLAYING,    // Active gameplay
    PAUSED,     // Game paused
    GAME_OVER   // Game over screen
};

class Game {
public:
    Game(int cols = 20, int rows = 20, int initialFoodCount = 2);

    // main tick: call each movement interval
    void Update();

    // flag to grow by 1 on next update
    void Grow();

    bool IsGameOver() const;
    bool IsPaused() const;
    void TogglePause();
    int GetScore() const;
    int GetLevel() const;
    int GetHighScore() const;

    const std::vector<Pos>& GetSnake() const;

    // returns vector of current food objects
    const std::vector<Food>& GetFoods() const;

    // returns vector of obstacle positions
    const std::vector<Pos>& GetObstacles() const;

    void Restart();

    void SetDirection(Dir d);

    // Game state management
    GameState GetState() const;
    void SetState(GameState state);
    void StartGame();  // Transition from MENU to PLAYING

private:
    void MoveHead();
    bool CheckSelfCollision(const Pos& newHead);
    bool CheckObstacleCollision(const Pos& pos);
    void LoadHighScore();
    void SaveHighScore();

    // recompute level & speed from score
    void RecalculateLevelAndSpeed();

    // generate obstacles based on current level
    void GenerateObstaclesForLevel(int level);

private:
    int m_cols, m_rows;
    std::vector<Pos> m_snake;
    Dir m_dir;
    bool m_grow;
    bool m_gameOver;
    bool m_paused;
    int m_score;
    int m_level;
    float m_speed; // seconds per step
    std::vector<Food> m_foods;
    std::vector<Pos> m_obstacles;

    int m_highScore;
    std::string m_highScoreFile;

    GameState m_state;  // Current game state
};