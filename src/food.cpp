#include "food.h"
#include <ctime>

Food::Food(int cols, int rows, int value, bool isPoison)
    : m_cols(cols), m_rows(rows), m_value(value), m_isPoison(isPoison), 
      m_visible(true), m_respawnCounter(0)
{
    // seed only first time; safe even if called multiple times
    static bool seeded = false;
    if (!seeded) { srand((unsigned)time(nullptr)); seeded = true; }
    Respawn({});
}

Pos Food::GetPosition() const {
    return m_pos;
}

int Food::GetValue() const {
    return m_value;
}

bool Food::IsPoison() const {
    return m_isPoison;
}

bool Food::IsVisible() const {
    return m_visible;
}

void Food::Respawn(const std::vector<Pos>& snake,
                   const std::vector<Pos>& otherFoodsPositions)
{
    // Poison food spawns less frequently (1 in 5 times)
    if (m_isPoison) {
        m_respawnCounter++;
        if (m_respawnCounter < 5) {
            m_visible = false;
            return; // Don't spawn yet
        }
        m_respawnCounter = 0; // Reset counter
        m_visible = true;
    }
    
    // try random positions until a free one is found (with a fallback)
    const int MAX_ATTEMPTS = 1000;
    int attempts = 0;
    while (attempts++ < MAX_ATTEMPTS) {
        m_pos.x = rand() % m_cols;
        m_pos.y = rand() % m_rows;

        bool conflict = false;
        for (const auto& p : snake) {
            if (p.x == m_pos.x && p.y == m_pos.y) { conflict = true; break; }
        }
        if (conflict) continue;

        for (const auto& p : otherFoodsPositions) {
            if (p.x == m_pos.x && p.y == m_pos.y) { conflict = true; break; }
        }
        if (!conflict) return;
    }

    // fallback: linear scan for free cell (deterministic)
    for (int y = 0; y < m_rows; ++y) {
        for (int x = 0; x < m_cols; ++x) {
            bool conflict = false;
            for (const auto& p : snake) if (p.x == x && p.y == y) { conflict = true; break; }
            for (const auto& p : otherFoodsPositions) if (p.x == x && p.y == y) { conflict = true; break; }
            if (!conflict) { m_pos.x = x; m_pos.y = y; return; }
        }
    }
    // if everything fails (very unlikely) keep previous position
}