cmake_minimum_required(VERSION 3.16)
project(SnakeGame LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(raylib CONFIG REQUIRED)

add_executable(snake 
    src/main.cpp
    src/game.cpp
    src/food.cpp
    src/input.cpp
)

target_include_directories(snake PRIVATE include)
target_link_libraries(snake PRIVATE raylib)

# Make MSVC link with main entry instead of WinMain
if (MSVC)
    target_link_options(snake PRIVATE "/ENTRY:mainCRTStartup")
endif()