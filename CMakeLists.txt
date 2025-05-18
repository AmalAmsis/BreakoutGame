cmake_minimum_required(VERSION 3.16)
project(BreakoutGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
        ${PROJECT_SOURCE_DIR}/engine
        ${PROJECT_SOURCE_DIR}/breakoutGame
        ${PROJECT_SOURCE_DIR}/libs/SDL/include
        ${PROJECT_SOURCE_DIR}/libs/SDL_image/include
        ${PROJECT_SOURCE_DIR}/libs/box2d/include
)

# Source files
set(SOURCES
        main.cpp
        breakoutGame/breakout_game.cpp
        )

# Add executable
add_executable(BreakoutGame ${SOURCES})

# Link library directories
target_link_directories(BreakoutGame PRIVATE
        ${PROJECT_SOURCE_DIR}/libs/SDL/lib
        ${PROJECT_SOURCE_DIR}/libs/SDL_image/lib
        ${PROJECT_SOURCE_DIR}/libs/box2d/lib
        )

# Link libraries
target_link_libraries(BreakoutGame
        SDL3
        SDL3_image
        box2d
        )
