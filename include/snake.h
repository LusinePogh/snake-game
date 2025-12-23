#pragma once
#include "pos.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Snake {
public:
    Snake(int cols, int rows);

    void Move(const Pos& dir);
    void Grow();
    bool CheckSelfCollision() const;
    const std::vector<Pos>& GetSegments() const;

    void SetDir(Pos dir);
    Pos GetDir() const;

private:
    int m_cols, m_rows;
    std::vector<Pos> m_segments;
    Pos m_direction;
    bool m_growNext;
};
