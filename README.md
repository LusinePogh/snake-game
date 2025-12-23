# Snake Game

A classic Snake game built with C++ and raylib, featuring levels, obstacles, and special food types.

## Features

- **5 Levels** with increasing difficulty and unique obstacle patterns
- **Multiple Food Types**:
  - Red circles: Regular food (+10 points)
  - Gold circles: Bonus food (+15 points)
  - Dark gray circles: Poison food (shrinks snake, spawns rarely)
- **Obstacles** that change with each level
- **Pause System** (P or SPACE)
- **High Score** tracking (saved to file)
- **Smooth Controls** (WASD or Arrow keys)

## How to Build

### Prerequisites
- CMake 3.16+
- C++17 compiler
- vcpkg (for raylib)

### Build Steps
```bash
# Configure
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build

# Run
./build/Debug/snake.exe
```

## Controls

- **Movement**: WASD or Arrow keys
- **Pause**: P or SPACE
- **Restart**: R

## Project Structure

```
snake-game/
├── include/           # Header files
│   ├── pos.h         # Position struct (shared)
│   ├── food.h        # Food class
│   ├── game.h        # Game logic
│   └── input.h       # Input handling
├── src/              # Source files
│   ├── main.cpp      # Entry point & rendering
│   ├── food.cpp      # Food implementation
│   ├── game.cpp      # Game logic
│   └── input.cpp     # Input processing
└── CMakeLists.txt    # Build configuration
```

## How It Works

### Game Loop
1. **Input** → Process keyboard input (movement, pause, restart)
2. **Update** → Move snake, check collisions, handle food
3. **Render** → Draw grid, obstacles, food, snake, and HUD

### Level System
- **Level 1**: 4 simple obstacles (50 points to advance)
- **Level 2**: L-shaped obstacles (100 points)
- **Level 3**: Wall corridors (150 points)
- **Level 4**: Cross pattern with narrow passages (200 points)
- **Level 5**: Complex maze (250 points)

Each level increases snake speed slightly.

### Food Types
- **Regular (Red)**: +10 points, grows snake by 1
- **Bonus (Gold)**: +15 points, grows snake by 1
- **Poison (Dark Gray)**: Shrinks snake by 1 segment, spawns 1/5 as often

### Collisions
- **Self-collision**: Game over
- **Obstacle collision**: Game over
- **Food collision**: Score points or shrink
- **Edge collision**: Wraps around to opposite side

## Code Architecture

### Core Classes

**`Game`** - Main game logic
- Manages snake, food, obstacles, score, level
- Handles movement and collision detection
- Saves/loads high scores

**`Food`** - Food entities
- Normal food and poison variants
- Spawn frequency control for poison
- Position management

**`InputHandler`** - Input processing
- Keyboard state checking
- Direction changes with anti-reverse logic
- Pause and restart handling

**`main.cpp`** - Rendering & game loop
- Window management with raylib
- Drawing all game elements
- Frame timing and game tick control
