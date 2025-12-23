#include "snake.h"

Snake::Snake(int cols, int rows)
    : m_cols(cols), m_rows(rows), m_direction({1,0}), m_growNext(false)
{
    // initial snake body (3 segments)
    m_segments.push_back({cols / 2, rows / 2});
    m_segments.push_back({cols / 2 - 1, rows / 2});
    m_segments.push_back({cols / 2 - 2, rows / 2});
}

void Snake::SetDir(Pos dir) {
    // simple no reverse logic
    if ((dir.x != -m_direction.x) || (dir.y != -m_direction.y)) {
        m_direction = dir;
    }
}

Pos Snake::GetDir() const {
    return m_direction;
}

void Snake::Move(const Pos& dir) {
    // compute new head
    Pos head = m_segments.front();
    Pos newHead = { head.x + dir.x, head.y + dir.y };

    // wrap-around (simple)
    if (newHead.x < 0) newHead.x = m_cols - 1;
    if (newHead.x >= m_cols) newHead.x = 0;
    if (newHead.y < 0) newHead.y = m_rows - 1;
    if (newHead.y >= m_rows) newHead.y = 0;

    m_segments.insert(m_segments.begin(), newHead);

    if (m_growNext) {
        m_growNext = false;
    } else {
        m_segments.pop_back();
    }
}

void Snake::Grow() {
    m_growNext = true;
}

bool Snake::CheckSelfCollision() const {
    const Pos& head = m_segments.front();
    for (size_t i = 1; i < m_segments.size(); ++i) {
        if (head.x == m_segments[i].x && head.y == m_segments[i].y) {
            return true;
        }
    }
    return false;
}

const std::vector<Pos>& Snake::GetSegments() const {
    return m_segments;
}
