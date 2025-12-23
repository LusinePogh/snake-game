#pragma once
#include <SFML/Graphics.hpp>
#include "snake.h"
#include "food.h"
#include <vector>

class RendererSFML {
public:
    RendererSFML(sf::RenderWindow& window, int cellSize);

    // multiple foods
    void Draw(const Snake& snake, const std::vector<Food>& foods,
              int score, int level, int highScore);

private:
    sf::RenderWindow& m_window;
    int m_cellSize;
};
