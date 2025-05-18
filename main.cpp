#include "breakoutGame/breakout_game.h"
#include "engine/bagel.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* sheet = nullptr;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_CreateWindowAndRenderer("Breakout ECS", 800, 600, 0, &window, &renderer);
    sheet = IMG_LoadTexture(renderer, "images/breakout.png");
    return window && renderer && sheet;
}

void setupEntities() {
    breakout::CreateUIManager();
    breakout::CreateBall();
    breakout::CreatePaddle(SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);

    for (int i = 0; i < 5; ++i) {
        bagel::Entity brick = bagel::Entity::create();
        brick.addAll(
            breakout::Position{50 + i * 150.0f, 100.0f},
            breakout::Sprite{2 + (i % 4) * 2},
            breakout::Collider{30},
            breakout::BrickHealth{2}
        );
    }

    bagel::Entity powerUp = bagel::Entity::create();
    powerUp.addAll(
        breakout::Position{350.0f, 150.0f},
        breakout::Velocity{0.0f, 2.0f},
        breakout::Sprite{10},
        breakout::Collider{12},
        breakout::PowerUpType{1},
        breakout::TimedEffect{5.0f}
    );
}

void gameLoop() {
    setupEntities();
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT ||
                (e.type == SDL_EVENT_KEY_DOWN && SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE])) {
                quit = true;
            }
        }

        breakout::PlayerControlSystem();
        breakout::MovementSystem();
        breakout::CollisionSystem();
        breakout::PowerUpSystem();
        breakout::DestroySystem();
        breakout::UISystem();
        breakout::RenderSystem(renderer, sheet);

        SDL_Delay(16);
        bagel::World::step();
    }
}

int main() {
    if (!initSDL()) return -1;

    gameLoop();

    SDL_DestroyTexture(sheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
