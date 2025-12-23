#pragma once
#include "pos.h"
#include <vector>
#include <cstdlib>

class Food {
public:
    // value = score gained when eaten (negative for poison)
    // isPoison = spawns less frequently (1 in 5 respawns)
    Food(int cols, int rows, int value = 10, bool isPoison = false);

    Pos GetPosition() const;
    int GetValue() const;
    bool IsPoison() const;
    bool IsVisible() const;

    // Respawn avoids positions occupied by snake and otherFoodsPositions
    void Respawn(const std::vector<Pos>& snake,
                 const std::vector<Pos>& otherFoodsPositions = {});

private:
    int m_cols, m_rows;
    Pos m_pos;
    int m_value;
    bool m_isPoison;
    bool m_visible;
    int m_respawnCounter; // For poison food spawn delay
};