#include "input.h"

bool InputHandler::IsMouseOverRect(int x, int y, int width, int height) {
    Vector2 mouse = GetMousePosition();
    return (mouse.x >= x && mouse.x <= x + width &&
            mouse.y >= y && mouse.y <= y + height);
}

// Processes input based on current game state
void InputHandler::Process(Game& game) {
    GameState state = game.GetState();
    
    switch (state) {
        case GameState::MENU: {
            // Menu input is handled in main.cpp for button clicks
            // Keyboard shortcut: Enter to start, Escape to exit
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                game.StartGame();
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                // Signal to exit (handled in main loop)
            }
            break;
        }
        
        case GameState::PLAYING: {
            // Arrow keys and WASD for direction
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    game.SetDirection(Dir::UP);
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  game.SetDirection(Dir::DOWN);
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  game.SetDirection(Dir::LEFT);
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) game.SetDirection(Dir::RIGHT);

            // Pause
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                game.TogglePause();
            }
            break;
        }
        
        case GameState::PAUSED: {
            // Resume with P, Escape, or Space
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
                game.TogglePause();
            }
            // Restart with R
            if (IsKeyPressed(KEY_R)) {
                game.StartGame();
            }
            // Return to menu with M
            if (IsKeyPressed(KEY_M)) {
                game.SetState(GameState::MENU);
            }
            break;
        }
        
        case GameState::GAME_OVER: {
            // Restart with R or Enter
            if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                game.StartGame();
            }
            // Return to menu with M or Escape
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) {
                game.SetState(GameState::MENU);
            }
            break;
        }
    }
}