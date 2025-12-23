// src/main.cpp
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "raylib.h"

#include "game.h"
#include "input.h"

// Simple grid settings
const int CELL = 24;
const int COLS = 20;
const int ROWS = 20;
const int WIDTH = COLS * CELL;
const int HEIGHT = ROWS * CELL + 80; // extra for HUD

static double lastUpdate = 0.0;
bool EventTriggered(double interval) {
    double t = GetTime();
    if (t - lastUpdate >= interval) { lastUpdate = t; return true; }
    return false;
}

int main() {
    // seed RNG (Game/Food also seed but harmless to do twice)
    srand((unsigned)time(nullptr));

    InitWindow(WIDTH, HEIGHT, "Snake Game");
    SetTargetFPS(60);

    // Use the Game class (defaults to 2 foods). Change third arg to start with different count.
    Game game(COLS, ROWS, 2);

    lastUpdate = GetTime();

    while (!WindowShouldClose()) {
        // Input handling (arrow keys, WASD, R restart) via InputHandler
        InputHandler::Process(game);

        // compute movement interval from level (same logic as Game::RecalculateLevelAndSpeed)
        int level = game.GetLevel();
        float base = 0.12f;
        float decrease = 0.01f * (level - 1);
        float interval = base - decrease;
        if (interval < 0.03f) interval = 0.03f;

        // Movement tick
        if (!game.IsGameOver() && !game.IsPaused() && EventTriggered(interval)) {
            game.Update();
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // subtle grid
            for (int x = 0; x < COLS; ++x) {
                for (int y = 0; y < ROWS; ++y) {
                    DrawRectangle(x*CELL, y*CELL, CELL, CELL, Fade(LIGHTGRAY, 0.08f));
                }
            }

            // draw obstacles (dark gray/black)
            const auto& obstacles = game.GetObstacles();
            for (const auto& obs : obstacles) {
                DrawRectangle(obs.x*CELL, obs.y*CELL, CELL, CELL, DARKGRAY);
                // Add a subtle border to make them stand out
                DrawRectangleLines(obs.x*CELL, obs.y*CELL, CELL, CELL, BLACK);
            }

            // draw foods (multiple) - now as circles
            const auto& foods = game.GetFoods();
            for (const auto& f : foods) {
                if (!f.IsVisible()) continue; // Skip invisible poison food
                
                Pos fp = f.GetPosition();
                int centerX = fp.x * CELL + CELL / 2;
                int centerY = fp.y * CELL + CELL / 2;
                int radius = CELL / 2 - 3;
                
                Color c;
                if (f.IsPoison()) {
                    c = DARKGRAY; // Poison food is dark gray
                } else {
                    c = (f.GetValue() > 10) ? GOLD : RED; // higher value = GOLD
                }
                
                DrawCircle(centerX, centerY, radius, c);
                
                // Add a subtle outline
                if (f.IsPoison()) {
                    DrawCircleLines(centerX, centerY, radius, BLACK);
                }
            }

            // draw snake: head darker, body lighter
            const auto& snake = game.GetSnake();
            for (size_t i = 0; i < snake.size(); ++i) {
                Color c = (i==0) ? BLUE : SKYBLUE;
                DrawRectangle(snake[i].x*CELL + 2, snake[i].y*CELL + 2, CELL-4, CELL-4, c);
            }

            // small "eye" on head — compute heading from first two segments (if available)
            if (snake.size() >= 2) {
                const Pos& head = snake.front();
                const Pos& neck = snake[1];
                int dx = head.x - neck.x;
                int dy = head.y - neck.y;
                // account for wrap-around case: prefer the direction with smaller absolute diff
                if (dx > 1) dx = dx - COLS;
                if (dx < -1) dx = dx + COLS;
                if (dy > 1) dy = dy - ROWS;
                if (dy < -1) dy = dy + ROWS;

                int cx = head.x*CELL + CELL/2;
                int cy = head.y*CELL + CELL/2;
                int eyeOffset = CELL/4;
                if (dx < 0) DrawCircle(cx - eyeOffset/1.5f, cy - eyeOffset/1.5f, 2, BLACK);
                else if (dx > 0) DrawCircle(cx + eyeOffset/1.5f, cy - eyeOffset/1.5f, 2, BLACK);
                else if (dy < 0) DrawCircle(cx - eyeOffset/1.5f, cy - eyeOffset/1.5f, 2, BLACK);
                else if (dy > 0) DrawCircle(cx + eyeOffset/1.5f, cy + eyeOffset/1.5f, 2, BLACK);
            }

            // HUD area
            DrawRectangle(0, ROWS*CELL, WIDTH, 80, DARKGRAY);
            DrawText(TextFormat("SCORE: %d", game.GetScore()), 8, ROWS*CELL + 8, 20, WHITE);
            DrawText(TextFormat("LENGTH: %d", (int)game.GetSnake().size()), 160, ROWS*CELL + 8, 20, WHITE);
            DrawText(TextFormat("LEVEL: %d", game.GetLevel()), 320, ROWS*CELL + 8, 20, WHITE);
            DrawText(TextFormat("BEST: %d", game.GetHighScore()), 8, ROWS*CELL + 36, 18, YELLOW);
            DrawText("R: Restart | P/SPACE: Pause | WASD/Arrows", 180, ROWS*CELL + 36, 14, WHITE);

            if (game.IsPaused()) {
                // Semi-transparent overlay for pause
                DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.5f));
                DrawText("PAUSED", WIDTH/2 - 80, HEIGHT/2 - 40, 40, YELLOW);
                DrawText("Press P or SPACE to continue", WIDTH/2 - 130, HEIGHT/2 + 10, 20, WHITE);
            }

            if (game.IsGameOver()) {
                // Semi-transparent overlay
                DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));
                DrawText("GAME OVER", WIDTH/2 - 100, HEIGHT/2 - 40, 40, RED);
                DrawText("Press R to restart", WIDTH/2 - 90, HEIGHT/2 + 10, 20, WHITE);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}