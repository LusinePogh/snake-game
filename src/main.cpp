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

// Button dimensions
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 50;

// Colors
const Color BUTTON_COLOR = { 70, 130, 180, 255 };        // Steel blue
const Color BUTTON_HOVER_COLOR = { 100, 149, 237, 255 }; // Cornflower blue
const Color BUTTON_TEXT_COLOR = WHITE;
const Color TITLE_COLOR = { 34, 139, 34, 255 };          // Forest green
const Color MENU_BG_COLOR = { 240, 248, 255, 255 };      // Alice blue

static double lastUpdate = 0.0;

bool EventTriggered(double interval) {
    double t = GetTime();
    if (t - lastUpdate >= interval) { lastUpdate = t; return true; }
    return false;
}

// Draw a button and return true if clicked
bool DrawButton(const char* text, int x, int y, int width, int height) {
    Vector2 mouse = GetMousePosition();
    bool hover = (mouse.x >= x && mouse.x <= x + width &&
                  mouse.y >= y && mouse.y <= y + height);
    
    Color bgColor = hover ? BUTTON_HOVER_COLOR : BUTTON_COLOR;
    
    // Draw button background with rounded corners effect (simple version)
    DrawRectangle(x, y, width, height, bgColor);
    DrawRectangleLines(x, y, width, height, DARKGRAY);
    
    // Draw shadow effect
    DrawRectangle(x + 3, y + 3, width, height, Fade(BLACK, 0.2f));
    DrawRectangle(x, y, width, height, bgColor);
    DrawRectangleLines(x, y, width, height, hover ? WHITE : DARKGRAY);
    
    // Center text
    int textWidth = MeasureText(text, 24);
    int textX = x + (width - textWidth) / 2;
    int textY = y + (height - 24) / 2;
    DrawText(text, textX, textY, 24, BUTTON_TEXT_COLOR);
    
    // Check for click
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void DrawMenu(Game& game) {
    ClearBackground(MENU_BG_COLOR);
    
    // Draw decorative snake pattern in background
    for (int i = 0; i < 5; ++i) {
        int x = 50 + i * 30;
        int y = 80;
        Color snakeColor = (i == 0) ? BLUE : SKYBLUE;
        DrawRectangle(x, y, CELL - 4, CELL - 4, Fade(snakeColor, 0.3f));
    }
    
    // Title
    const char* title = "SNAKE GAME";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (WIDTH - titleWidth) / 2, 100, 60, TITLE_COLOR);
    
    // Subtitle
    const char* subtitle = "Classic Arcade Game";
    int subWidth = MeasureText(subtitle, 20);
    DrawText(subtitle, (WIDTH - subWidth) / 2, 170, 20, DARKGRAY);
    
    // High Score display
    char highScoreText[64];
    sprintf(highScoreText, "High Score: %d", game.GetHighScore());
    int hsWidth = MeasureText(highScoreText, 18);
    DrawText(highScoreText, (WIDTH - hsWidth) / 2, 210, 18, GOLD);
    
    // Buttons
    int buttonX = (WIDTH - BUTTON_WIDTH) / 2;
    int startY = 270;
    int exitY = 340;
    
    if (DrawButton("START GAME", buttonX, startY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        game.StartGame();
    }
    
    if (DrawButton("EXIT", buttonX, exitY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        CloseWindow();
        exit(0);
    }
    
    // Instructions
    DrawText("Controls:", 20, HEIGHT - 70, 16, DARKGRAY);
    DrawText("WASD / Arrows: Move | P: Pause | R: Restart", 20, HEIGHT - 50, 14, GRAY);
    DrawText("Press ENTER to start", (WIDTH - MeasureText("Press ENTER to start", 14)) / 2, HEIGHT - 25, 14, DARKGRAY);
}

void DrawGame(Game& game) {
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
        DrawRectangleLines(obs.x*CELL, obs.y*CELL, CELL, CELL, BLACK);
    }

    // draw foods (multiple) - now as circles
    const auto& foods = game.GetFoods();
    for (const auto& f : foods) {
        if (!f.IsVisible()) continue;
        
        Pos fp = f.GetPosition();
        int centerX = fp.x * CELL + CELL / 2;
        int centerY = fp.y * CELL + CELL / 2;
        int radius = CELL / 2 - 3;
        
        Color c;
        if (f.IsPoison()) {
            c = DARKGRAY;
        } else {
            c = (f.GetValue() > 10) ? GOLD : RED;
        }
        
        DrawCircle(centerX, centerY, radius, c);
        
        if (f.IsPoison()) {
            DrawCircleLines(centerX, centerY, radius, BLACK);
        }
    }

    // draw snake
    const auto& snake = game.GetSnake();
    for (size_t i = 0; i < snake.size(); ++i) {
        Color c = (i == 0) ? BLUE : SKYBLUE;
        DrawRectangle(snake[i].x*CELL + 2, snake[i].y*CELL + 2, CELL-4, CELL-4, c);
    }

    // snake eye
    if (snake.size() >= 2) {
        const Pos& head = snake.front();
        const Pos& neck = snake[1];
        int dx = head.x - neck.x;
        int dy = head.y - neck.y;
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
    DrawText("P: Pause | M: Menu", 180, ROWS*CELL + 36, 14, WHITE);
}

void DrawPauseOverlay(Game& game) {
    DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.5f));
    
    const char* title = "PAUSED";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, (WIDTH - titleWidth) / 2, HEIGHT/2 - 60, 40, YELLOW);
    
    // Buttons
    int buttonX = (WIDTH - BUTTON_WIDTH) / 2;
    
    if (DrawButton("RESUME", buttonX, HEIGHT/2 - 10, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        game.TogglePause();
    }
    
    if (DrawButton("MAIN MENU", buttonX, HEIGHT/2 + 50, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        game.SetState(GameState::MENU);
    }
    
    DrawText("Press P or ESC to resume", (WIDTH - MeasureText("Press P or ESC to resume", 16)) / 2, HEIGHT/2 + 115, 16, WHITE);
}

void DrawGameOverOverlay(Game& game) {
    DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));
    
    const char* title = "GAME OVER";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, (WIDTH - titleWidth) / 2, HEIGHT/2 - 100, 40, RED);
    
    // Final score
    char scoreText[64];
    sprintf(scoreText, "Final Score: %d", game.GetScore());
    int scoreWidth = MeasureText(scoreText, 24);
    DrawText(scoreText, (WIDTH - scoreWidth) / 2, HEIGHT/2 - 50, 24, WHITE);
    
    // New high score?
    if (game.GetScore() >= game.GetHighScore() && game.GetScore() > 0) {
        const char* newHigh = "NEW HIGH SCORE!";
        int nhWidth = MeasureText(newHigh, 20);
        DrawText(newHigh, (WIDTH - nhWidth) / 2, HEIGHT/2 - 20, 20, GOLD);
    }
    
    // Buttons
    int buttonX = (WIDTH - BUTTON_WIDTH) / 2;
    
    if (DrawButton("PLAY AGAIN", buttonX, HEIGHT/2 + 20, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        game.StartGame();
    }
    
    if (DrawButton("MAIN MENU", buttonX, HEIGHT/2 + 80, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        game.SetState(GameState::MENU);
    }
    
    DrawText("Press R to restart | M for menu", (WIDTH - MeasureText("Press R to restart | M for menu", 14)) / 2, HEIGHT/2 + 145, 14, LIGHTGRAY);
}

int main() {
    srand((unsigned)time(nullptr));

    InitWindow(WIDTH, HEIGHT, "Snake Game");
    SetTargetFPS(60);

    Game game(COLS, ROWS, 2);

    lastUpdate = GetTime();

    while (!WindowShouldClose()) {
        // Process input based on game state
        InputHandler::Process(game);
        
        // Handle ESC in menu to exit
        if (game.GetState() == GameState::MENU && IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        // Update game logic only when playing
        if (game.GetState() == GameState::PLAYING) {
            int level = game.GetLevel();
            float base = 0.12f;
            float decrease = 0.01f * (level - 1);
            float interval = base - decrease;
            if (interval < 0.03f) interval = 0.03f;

            if (EventTriggered(interval)) {
                game.Update();
            }
        }

        // Draw
        BeginDrawing();
        
        switch (game.GetState()) {
            case GameState::MENU:
                DrawMenu(game);
                break;
                
            case GameState::PLAYING:
                DrawGame(game);
                break;
                
            case GameState::PAUSED:
                DrawGame(game);
                DrawPauseOverlay(game);
                break;
                
            case GameState::GAME_OVER:
                DrawGame(game);
                DrawGameOverOverlay(game);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}