#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

#include "include/draw.hpp"
#include "include/body.hpp"


int main() {
    draw drawer;
    drawer.clear();

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 9.81f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    body box = body(worldId, {200.0f, 200.0f}, 50.0f, 50.0f, b2_dynamicBody);
    drawer.addShape(box);

    body ground = body(worldId, {200.0f, 500.0f}, 800.0f, 50.0f, b2_staticBody);
    drawer.addShape(ground);

    // tick world
    const float timeStep = 1.0f / 60.0f;
    const int32_t velocityIterations = 6;
    const int32_t positionIterations = 2;
    for (int32_t i = 0; i < 60; ++i) {
        b2World_Step(worldId, timeStep, velocityIterations);
    }
    drawer.drawAll();
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
