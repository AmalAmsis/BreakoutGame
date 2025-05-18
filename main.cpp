#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LASER_FIRE_DELAY = 60;

const SDL_FRect BALL_SRC     = {800, 548, 87, 77};
const SDL_FRect PADDLE_SRC   = {392, 9, 161, 55};

const SDL_FRect BRICK_BLUE       = {21, 17, 171, 59};
const SDL_FRect BRICK_BLUE_DMG   = {209, 16, 171, 60};
const SDL_FRect BRICK_PURPLE     = {20, 169, 168, 57};
const SDL_FRect BRICK_PURPLE_DMG = {208, 168, 170, 58};
const SDL_FRect BRICK_YELLOW     = {20, 469, 169, 59};
const SDL_FRect BRICK_YELLOW_DMG = {210, 470, 166, 63};
const SDL_FRect BRICK_ORANGE     = {17, 319, 175, 57};
const SDL_FRect BRICK_ORANGE_DMG = {206, 318, 175, 58};

const SDL_FRect LASER_SPRITE     = {837, 643, 11, 22};

enum BrickState { NORMAL, DAMAGED, DESTROYED };

/**
 * @brief Represents a single brick with animation state.
 */
struct Brick {
    SDL_FRect dest;                         ///< On-screen rectangle
    const SDL_FRect* spriteNormal;         ///< Default sprite
    const SDL_FRect* spriteBroken;         ///< Damaged sprite
    BrickState state = NORMAL;             ///< Current visual state
    int hitTimer = 0;                      ///< Timer until it breaks
    bool animate = false;                  ///< Whether this brick animates
};

/**
 * @brief Represents a laser beam that moves upward.
 */
struct LaserBeam {
    SDL_FRect dest;        ///< On-screen rectangle
    float speedY = -5.0f;  ///< Upward movement speed
    bool active = true;    ///< Is the laser visible/active
};

/**
 * @brief Initializes SDL, window, renderer, and texture.
 *
 * @param window Reference to window pointer to be initialized.
 * @param renderer Reference to renderer pointer to be initialized.
 * @param sheet Reference to texture pointer to be initialized.
 * @return true if initialization succeeded, false otherwise.
 */
bool init(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& sheet) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_CreateWindowAndRenderer("Breakout POC", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    if (!window || !renderer) {
        std::cerr << "Failed to create window or renderer\n";
        return false;
    }
    sheet = IMG_LoadTexture(renderer, "images/breakout.png");
    if (!sheet) {
        std::cerr << "Failed to load texture\n";
        return false;
    }
    return true;
}

/**
 * @brief Creates and returns a grid of bricks with different sprites.
 *
 * @return Vector of Brick objects.
 */
std::vector<Brick> createBricks() {
    std::vector<Brick> bricks;
    const int ROWS = 6, COLS = 5;
    const float spacing = 10.0f, brickW = 120.0f, brickH = 30.0f;

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            Brick b;
            b.dest = {50.0f + col * (brickW + spacing), 80.0f + row * (brickH + spacing), brickW, brickH};
            if (row == 0) {
                b.spriteNormal = &BRICK_BLUE; b.spriteBroken = &BRICK_BLUE_DMG;
            } else if (row == 1) {
                b.spriteNormal = &BRICK_PURPLE; b.spriteBroken = &BRICK_PURPLE_DMG;
            } else if (row == 2) {
                b.spriteNormal = &BRICK_YELLOW; b.spriteBroken = &BRICK_YELLOW_DMG;
            } else {
                b.spriteNormal = &BRICK_ORANGE; b.spriteBroken = &BRICK_ORANGE_DMG;
                b.animate = true;
            }
            bricks.push_back(b);
        }
    }
    return bricks;
}

/**
 * @brief Updates ball position and handles bouncing.
 *
 * @param ball Reference to ball rectangle.
 * @param vx Velocity in x direction.
 * @param vy Velocity in y direction.
 */
void updateBall(SDL_FRect& ball, float& vx, float& vy) {
    ball.x += vx;
    ball.y += vy;
    if (ball.x < 0 || ball.x + ball.w > SCREEN_WIDTH) vx *= -1;
    if (ball.y < 0 || ball.y + ball.h > SCREEN_HEIGHT) vy *= -1;
}

/**
 * @brief Moves paddle automatically in a ping-pong style.
 *
 * @param paddle Reference to paddle rectangle.
 * @param dir Direction of movement.
 */
void updatePaddle(SDL_FRect& paddle, float& dir) {
    paddle.x += dir * 3;
    if (paddle.x < 0 || paddle.x + paddle.w > SCREEN_WIDTH) dir *= -1;
}

/**
 * @brief Fires two laser beams from paddle.
 *
 * @param lasers Vector of lasers to add to.
 * @param paddle Paddle rectangle to calculate origin.
 */
void fireLasers(std::vector<LaserBeam>& lasers, const SDL_FRect& paddle) {
    float xLeft = paddle.x + 10;
    float xRight = paddle.x + paddle.w - 10 - LASER_SPRITE.w;
    float y = paddle.y;
    lasers.push_back({{xLeft, y, LASER_SPRITE.w, LASER_SPRITE.h}});
    lasers.push_back({{xRight, y, LASER_SPRITE.w, LASER_SPRITE.h}});
}

/**
 * @brief Updates lasers and removes inactive ones.
 *
 * @param lasers Vector of active laser beams.
 */
void updateLasers(std::vector<LaserBeam>& lasers) {
    for (auto& l : lasers) {
        if (!l.active) continue;
        l.dest.y += l.speedY;
        if (l.dest.y + l.dest.h < 0) l.active = false;
    }
}

/**
 * @brief Animates bricks with timed transitions.
 *
 * @param bricks Vector of bricks to update.
 */
void animateBricks(std::vector<Brick>& bricks) {
    for (auto& b : bricks) {
        if (b.animate && b.state != DESTROYED) {
            b.hitTimer++;
            if (b.hitTimer > 45 && b.state == NORMAL) {
                b.state = DAMAGED;
                b.hitTimer = 0;
            } else if (b.hitTimer > 46 && b.state == DAMAGED) {
                b.state = DESTROYED;
            }
        }
    }
}

/**
 * @brief Renders all game elements: ball, paddle, bricks, and lasers.
 *
 * @param renderer SDL renderer.
 * @param sheet Texture sheet.
 * @param ball Ball rectangle.
 * @param paddle Paddle rectangle.
 * @param bricks Vector of bricks.
 * @param lasers Vector of lasers.
 */
void renderAll(SDL_Renderer* renderer, SDL_Texture* sheet, const SDL_FRect& ball, const SDL_FRect& paddle, const std::vector<Brick>& bricks, const std::vector<LaserBeam>& lasers) {
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, sheet, &BALL_SRC, &ball);
    SDL_RenderTexture(renderer, sheet, &PADDLE_SRC, &paddle);
    for (const auto& b : bricks) {
        if (b.state == DESTROYED) continue;
        const SDL_FRect* sprite = (b.state == DAMAGED) ? b.spriteBroken : b.spriteNormal;
        SDL_RenderTexture(renderer, sheet, sprite, &b.dest);
    }
    for (const auto& l : lasers) {
        if (l.active)
            SDL_RenderTexture(renderer, sheet, &LASER_SPRITE, &l.dest);
    }
}

/**
 * @brief Runs the main animation loop of the game.
 *
 * This function simulates movement of the ball and paddle,
 * fires lasers, animates bricks, and renders the game frame by frame.
 *
 * @param renderer The SDL renderer used to draw.
 * @param sheet The sprite sheet texture used for rendering game elements.
 */
void runGameLoop(SDL_Renderer* renderer, SDL_Texture* sheet) {
    SDL_FRect ball = {100.0f, 100.0f, 40.0f, 40.0f};
    SDL_FRect paddle = {300.0f, 550.0f, 140.0f, 40.0f};
    float ballVX = 2.5f, ballVY = -2.0f, paddleDir = 1.0f;
    auto bricks = createBricks();
    std::vector<LaserBeam> lasers;
    int laserFireTimer = 0;

    for (int frame = 0; frame < 500; ++frame) {
        updateBall(ball, ballVX, ballVY);
        updatePaddle(paddle, paddleDir);
        if (++laserFireTimer >= LASER_FIRE_DELAY) {
            laserFireTimer = 0;
            fireLasers(lasers, paddle);
        }
        updateLasers(lasers);
        animateBricks(bricks);
        renderAll(renderer, sheet, ball, paddle, bricks, lasers);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

/**
 * @brief Cleans up SDL resources used by the game.
 *
 * Frees the SDL texture, renderer, and window, and calls SDL_Quit().
 *
 * @param window The SDL window to destroy.
 * @param renderer The SDL renderer to destroy.
 * @param sheet The texture to destroy.
 */
void cleanUp(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* sheet) {
    SDL_DestroyTexture(sheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief The main function and entry point of the game.
 *
 * Initializes SDL components and starts the game loop.
 *
 * @return int Exit code (0 for success, -1 for failure).
 */
int main() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* sheet = nullptr;
    if (!init(window, renderer, sheet)) return -1;

    runGameLoop(renderer, sheet);
    cleanUp(window, renderer, sheet);
    return 0;
}