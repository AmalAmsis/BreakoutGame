#include <box2d/box2d.h>
#include <iostream>

int main() {
    // Create world definition with gravity
    b2WorldDef worldDef{};
    worldDef.gravity = {0.0f, -9.8f};

    // Create the world and get its ID
    b2WorldId worldId = b2CreateWorld(&worldDef);

    std::cout << "Box2D world created!" << std::endl;

    // Destroy the world when done
    b2DestroyWorld(worldId);
    return 0;
}
