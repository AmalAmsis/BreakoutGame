cmake_minimum_required(VERSION 3.16)
project(BreakoutGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# === Include directories ===
include_directories(
        ${PROJECT_SOURCE_DIR}/engine
        ${PROJECT_SOURCE_DIR}/breakoutGame

        # SDL3 include
        ${PROJECT_SOURCE_DIR}/libs/SDL3-3.2.14/x86_64-w64-mingw32/include

        # SDL_image include
        ${PROJECT_SOURCE_DIR}/libs/SDL3_image-3.2.4/x86_64-w64-mingw32/include
)

# === Source files ===
set(SOURCES
        main.cpp
        breakoutGame/breakout_game.cpp
)

# === Add executable ===
add_executable(BreakoutGame ${SOURCES})

# === Link library directories ===
target_link_directories(BreakoutGame PRIVATE
        ${PROJECT_SOURCE_DIR}/libs/SDL3-3.2.14/x86_64-w64-mingw32/lib
        ${PROJECT_SOURCE_DIR}/libs/SDL3_image-3.2.4/x86_64-w64-mingw32/lib
)

# === Link libraries ===
target_link_libraries(BreakoutGame
        SDL3
        SDL3_image
)
