#pragma once
#include "raylib.h"
#include "game.h"

class InputHandler {
public:
    // Process input based on current game state
    static void Process(Game& game);
    
    // Check if mouse is over a rectangle (for button hover)
    static bool IsMouseOverRect(int x, int y, int width, int height);
};