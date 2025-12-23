#include "input.h"

// Processes arrow keys + WASD. Also handles restart (R) and pause (P/Space).
void InputHandler::Process(Game& game) {
    // Arrow keys
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    game.SetDirection(Dir::UP);
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  game.SetDirection(Dir::DOWN);
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  game.SetDirection(Dir::LEFT);
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) game.SetDirection(Dir::RIGHT);

    // quick restart
    if (IsKeyPressed(KEY_R)) {
        game.Restart();
    }
    
    // pause/unpause
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
        game.TogglePause();
    }
}