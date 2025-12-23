#include "renderer_sfml.h"
#include <string>

RendererSFML::RendererSFML(sf::RenderWindow& window, int cellSize)
    : m_window(window), m_cellSize(cellSize) {}

void RendererSFML::Draw(const Snake& snake, const std::vector<Food>& foods,
                        int score, int level, int highScore)
{
    m_window.clear(sf::Color::Black);

    // draw foods
    sf::RectangleShape foodRect(sf::Vector2f(m_cellSize - 2, m_cellSize - 2));
    for (const auto& f : foods) {
        foodRect.setPosition(f.GetPosition().x * m_cellSize + 1,
                             f.GetPosition().y * m_cellSize + 1);
        foodRect.setFillColor(sf::Color::Red);
        m_window.draw(foodRect);
    }

    // draw snake
    const auto& segments = snake.GetSegments();
    for (size_t i = 0; i < segments.size(); ++i) {
        sf::RectangleShape segRect(sf::Vector2f(m_cellSize - 2, m_cellSize - 2));
        segRect.setPosition(segments[i].x * m_cellSize + 1,
                            segments[i].y * m_cellSize + 1);
        segRect.setFillColor((i == 0) ? sf::Color::Blue : sf::Color::Cyan);
        m_window.draw(segRect);
    }

    // simple HUD (score / level / highscore)
    sf::Text hud;
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) { fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf"); } // fallback
    if (fontLoaded) {
        hud.setFont(font);
        hud.setCharacterSize(18);
        hud.setFillColor(sf::Color::White);
        hud.setString("Score: " + std::to_string(score) +
                      "  Level: " + std::to_string(level) +
                      "  HighScore: " + std::to_string(highScore));
        hud.setPosition(5, m_window.getSize().y - 25);
        m_window.draw(hud);
    }

    m_window.display();
}
