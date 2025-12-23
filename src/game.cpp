#include "game.h"
#include <algorithm>
#include <fstream>
#include <ctime>

Game::Game(int cols, int rows, int initialFoodCount)
    : m_cols(cols),
      m_rows(rows),
      m_dir(Dir::RIGHT),
      m_grow(false),
      m_gameOver(false),
      m_paused(false),
      m_score(0),
      m_level(1),
      m_speed(0.12f),
      m_highScore(0),
      m_highScoreFile("highscore.txt"),
      m_state(GameState::MENU)  // Start in menu
{
    srand((unsigned)time(nullptr));
    // create initial foods (different values possible)
    // Include 1 poison food (spawns 5x less often via respawn logic)
    m_foods.clear();
    for (int i = 0; i < initialFoodCount; ++i) {
        int val = (i == 0) ? 10 : 15; // second food a bit more valuable
        m_foods.emplace_back(m_cols, m_rows, val);
    }
    // Add one poison food (negative value, spawns less frequently)
    m_foods.emplace_back(m_cols, m_rows, -10, true); // true = poison food
    
    LoadHighScore();
    // Don't call Restart() here - wait for user to start from menu
    
    // Initialize snake for preview (optional)
    m_snake.clear();
    m_snake.push_back({m_cols / 2, m_rows / 2});
    m_snake.push_back({m_cols / 2 - 1, m_rows / 2});
    m_snake.push_back({m_cols / 2 - 2, m_rows / 2});
}

void Game::Restart() {
    m_snake.clear();
    m_snake.push_back({m_cols / 2, m_rows / 2});
    m_snake.push_back({m_cols / 2 - 1, m_rows / 2});
    m_snake.push_back({m_cols / 2 - 2, m_rows / 2});
    m_dir = Dir::RIGHT;
    m_grow = false;
    m_gameOver = false;
    m_paused = false;
    m_score = 0;
    m_level = 1;
    m_speed = 0.12f;

    // Generate obstacles for level 1
    GenerateObstaclesForLevel(m_level);

    // Respawn all foods ensuring no overlap with snake, obstacles, and between foods
    std::vector<Pos> other;
    for (size_t i = 0; i < m_foods.size(); ++i) {
        std::vector<Pos> others;
        for (size_t j = 0; j < m_foods.size(); ++j) {
            if (i == j) continue;
            others.push_back(m_foods[j].GetPosition());
        }
        // Add obstacles to the list of positions to avoid
        for (const auto& obs : m_obstacles) {
            others.push_back(obs);
        }
        m_foods[i].Respawn(m_snake, others);
    }
}

void Game::StartGame() {
    Restart();
    m_state = GameState::PLAYING;
}

GameState Game::GetState() const {
    return m_state;
}

void Game::SetState(GameState state) {
    m_state = state;
}

void Game::SetDirection(Dir d) {
    // no reverse
    if ((d == Dir::UP && m_dir == Dir::DOWN) ||
        (d == Dir::DOWN && m_dir == Dir::UP) ||
        (d == Dir::LEFT && m_dir == Dir::RIGHT) ||
        (d == Dir::RIGHT && m_dir == Dir::LEFT))
        return;

    m_dir = d;
}

void Game::Update() {
    if (m_state != GameState::PLAYING) return;
    if (m_gameOver || m_paused) return;

    MoveHead();

    Pos head = m_snake.front();

    // check each food
    for (size_t i = 0; i < m_foods.size(); ++i) {
        auto fpos = m_foods[i].GetPosition();
        if (head.x == fpos.x && head.y == fpos.y) {
            int foodValue = m_foods[i].GetValue();
            
            if (foodValue < 0) {
                // Poison food - shrink snake
                int shrinkAmount = -foodValue / 10; // e.g., -10 value = shrink by 1
                for (int j = 0; j < shrinkAmount && m_snake.size() > 3; ++j) {
                    m_snake.pop_back();
                }
            } else {
                // Regular food - grow and score
                m_score += foodValue;
                m_grow = true;
            }
            
            // respawn this food, provide positions of other foods and obstacles so it won't overlap
            std::vector<Pos> others;
            for (size_t j = 0; j < m_foods.size(); ++j) {
                if (i == j) continue;
                others.push_back(m_foods[j].GetPosition());
            }
            // Add obstacles
            for (const auto& obs : m_obstacles) {
                others.push_back(obs);
            }
            m_foods[i].Respawn(m_snake, others);
        }
    }

    if (!m_grow)
        m_snake.pop_back();  // move (no grow)
    else
        m_grow = false;      // reset grow

    // after moving / eating, recalc level & speed
    RecalculateLevelAndSpeed();
}

void Game::Grow() {
    m_grow = true;
}

bool Game::CheckSelfCollision(const Pos& newHead) {
    for (size_t i = 0; i < m_snake.size(); ++i) {
        if (m_snake[i].x == newHead.x &&
            m_snake[i].y == newHead.y)
        {
            return true;
        }
    }
    return false;
}

bool Game::CheckObstacleCollision(const Pos& pos) {
    for (const auto& obs : m_obstacles) {
        if (obs.x == pos.x && obs.y == pos.y) {
            return true;
        }
    }
    return false;
}

void Game::MoveHead() {
    Pos newHead = m_snake.front();

    switch (m_dir) {
    case Dir::UP:    newHead.y -= 1; break;
    case Dir::DOWN:  newHead.y += 1; break;
    case Dir::LEFT:  newHead.x -= 1; break;
    case Dir::RIGHT: newHead.x += 1; break;
    }

    // wrap around screen (simple)
    if (newHead.x < 0)         newHead.x = m_cols - 1;
    else if (newHead.x >= m_cols) newHead.x = 0;

    if (newHead.y < 0)         newHead.y = m_rows - 1;
    else if (newHead.y >= m_rows) newHead.y = 0;

    // Check collisions
    if (CheckSelfCollision(newHead) || CheckObstacleCollision(newHead)) {
        m_gameOver = true;
        m_state = GameState::GAME_OVER;
        // save high score if beaten
        if (m_score > m_highScore) {
            m_highScore = m_score;
            SaveHighScore();
        }
    } else {
        m_snake.insert(m_snake.begin(), newHead);
    }
}

bool Game::IsGameOver() const { return m_gameOver; }
bool Game::IsPaused() const { return m_paused; }
void Game::TogglePause() { 
    if (m_state == GameState::PLAYING && !m_gameOver) {
        m_paused = !m_paused;
        m_state = m_paused ? GameState::PAUSED : GameState::PLAYING;
    } else if (m_state == GameState::PAUSED) {
        m_paused = false;
        m_state = GameState::PLAYING;
    }
}
int Game::GetScore() const { return m_score; }
int Game::GetLevel() const { return m_level; }
int Game::GetHighScore() const { return m_highScore; }
const std::vector<Pos>& Game::GetSnake() const { return m_snake; }
const std::vector<Food>& Game::GetFoods() const { return m_foods; }
const std::vector<Pos>& Game::GetObstacles() const { return m_obstacles; }

void Game::LoadHighScore() {
    std::ifstream in(m_highScoreFile);
    if (!in) { m_highScore = 0; return; }
    int v = 0;
    in >> v;
    if (in) m_highScore = v;
    else m_highScore = 0;
}

void Game::SaveHighScore() {
    std::ofstream out(m_highScoreFile, std::ios::trunc);
    if (!out) return;
    out << m_highScore;
}

void Game::RecalculateLevelAndSpeed() {
    // example: every 50 points -> +1 level (max level 5)
    int newLevel = 1 + (m_score / 50);
    if (newLevel > 5) newLevel = 5; // cap at level 5
    
    if (newLevel != m_level) {
        m_level = newLevel;
        
        // Generate new obstacles for the new level
        GenerateObstaclesForLevel(m_level);
        
        // Respawn foods to avoid new obstacles
        for (size_t i = 0; i < m_foods.size(); ++i) {
            std::vector<Pos> others;
            for (size_t j = 0; j < m_foods.size(); ++j) {
                if (i == j) continue;
                others.push_back(m_foods[j].GetPosition());
            }
            for (const auto& obs : m_obstacles) {
                others.push_back(obs);
            }
            // Check if current food is on obstacle, respawn if needed
            bool onObstacle = false;
            auto fpos = m_foods[i].GetPosition();
            for (const auto& obs : m_obstacles) {
                if (obs.x == fpos.x && obs.y == fpos.y) {
                    onObstacle = true;
                    break;
                }
            }
            if (onObstacle) {
                m_foods[i].Respawn(m_snake, others);
            }
        }
        
        // optional: add a new food every few levels (keep it simple)
        if (m_level > 1 && (m_level % 3) == 0 && m_foods.size() < 5) {
            // push a new slightly valuable food
            m_foods.emplace_back(m_cols, m_rows, 20);
            // respawn it not on snake, other foods, or obstacles
            std::vector<Pos> otherPositions;
            for (size_t i = 0; i + 1 < m_foods.size(); ++i)
                otherPositions.push_back(m_foods[i].GetPosition());
            for (const auto& obs : m_obstacles)
                otherPositions.push_back(obs);
            m_foods.back().Respawn(m_snake, otherPositions);
        }
    }

    // speed reduces a bit with level, clamp to a minimum
    float base = 0.12f;
    float decrease = 0.01f * (m_level - 1); // -0.01 per level
    m_speed = base - decrease;
    if (m_speed < 0.03f) m_speed = 0.03f;
}

void Game::GenerateObstaclesForLevel(int level) {
    m_obstacles.clear();
    
    // Helper lambda to add obstacle avoiding center spawn area
    auto addObstacle = [&](int x, int y) {
        // Avoid center area where snake spawns (cols/2 - 3 to cols/2 + 3)
        int centerX = m_cols / 2;
        int centerY = m_rows / 2;
        if (x >= centerX - 3 && x <= centerX + 3 && 
            y >= centerY - 2 && y <= centerY + 2) {
            return; // Skip center spawn area
        }
        if (x >= 0 && x < m_cols && y >= 0 && y < m_rows) {
            m_obstacles.push_back({x, y});
        }
    };
    
    switch (level) {
        case 1: {
            // Level 1: Simple 2x1 obstacles scattered around
            addObstacle(3, 3);
            addObstacle(4, 3);
            
            addObstacle(15, 5);
            addObstacle(16, 5);
            
            addObstacle(7, 15);
            addObstacle(8, 15);
            
            addObstacle(12, 10);
            addObstacle(13, 10);
            break;
        }
        
        case 2: {
            // Level 2: L-shaped obstacles and more blocks
            // L-shape 1
            addObstacle(2, 2);
            addObstacle(3, 2);
            addObstacle(2, 3);
            
            // L-shape 2
            addObstacle(16, 4);
            addObstacle(17, 4);
            addObstacle(17, 5);
            
            // Horizontal bars
            addObstacle(5, 12);
            addObstacle(6, 12);
            addObstacle(7, 12);
            
            addObstacle(13, 15);
            addObstacle(14, 15);
            addObstacle(15, 15);
            break;
        }
        
        case 3: {
            // Level 3: Partial walls creating corridors
            // Left wall
            for (int y = 2; y < 8; ++y) {
                addObstacle(4, y);
            }
            
            // Right wall
            for (int y = 12; y < 18; ++y) {
                addObstacle(15, y);
            }
            
            // Top horizontal
            for (int x = 8; x < 13; ++x) {
                addObstacle(x, 3);
            }
            
            // Bottom horizontal
            for (int x = 7; x < 12; ++x) {
                addObstacle(x, 16);
            }
            break;
        }
        
        case 4: {
            // Level 4: Narrow passages with cross pattern
            // Vertical bars
            for (int y = 1; y < 9; ++y) {
                addObstacle(6, y);
            }
            for (int y = 11; y < 19; ++y) {
                addObstacle(13, y);
            }
            
            // Horizontal bars
            for (int x = 1; x < 7; ++x) {
                addObstacle(x, 10);
            }
            for (int x = 13; x < 19; ++x) {
                addObstacle(x, 10);
            }
            
            // Corner blocks
            addObstacle(2, 2);
            addObstacle(2, 3);
            addObstacle(17, 2);
            addObstacle(17, 3);
            addObstacle(2, 17);
            addObstacle(2, 18);
            addObstacle(17, 17);
            addObstacle(17, 18);
            break;
        }
        
        case 5: {
            // Level 5: Complex maze-like pattern
            // Outer frame (partial)
            for (int x = 1; x < 6; ++x) {
                addObstacle(x, 1);
                addObstacle(x, 18);
            }
            for (int x = 14; x < 19; ++x) {
                addObstacle(x, 1);
                addObstacle(x, 18);
            }
            for (int y = 1; y < 6; ++y) {
                addObstacle(1, y);
                addObstacle(18, y);
            }
            for (int y = 14; y < 19; ++y) {
                addObstacle(1, y);
                addObstacle(18, y);
            }
            
            // Internal maze walls
            for (int y = 5; y < 10; ++y) {
                addObstacle(7, y);
            }
            for (int y = 10; y < 15; ++y) {
                addObstacle(12, y);
            }
            for (int x = 4; x < 9; ++x) {
                addObstacle(x, 7);
            }
            for (int x = 11; x < 16; ++x) {
                addObstacle(x, 13);
            }
            
            // Center blockers
            addObstacle(9, 9);
            addObstacle(10, 9);
            addObstacle(9, 10);
            addObstacle(10, 10);
            break;
        }
        
        default:
            // Level 5+ uses level 5 pattern
            GenerateObstaclesForLevel(5);
            break;
    }
}