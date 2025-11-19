#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <csignal>
#include <atomic>

#include "include/draw.hpp"
#include "include/body.hpp"

volatile std::sig_atomic_t g_stop = 0;
extern "C" void sigint_handler(int) {
    g_stop = 1; // only set a sig_atomic_t
}

int main() {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);
    sf::Event event = sf::Event::Closed();

    unsigned int windowWidth = 800;
    unsigned int windowHeight = 600;

    float halfHeight = windowHeight / 2.0f;
    float halfWidth = windowWidth / 2.0f;

    // Create Drawer
    draw drawer(windowWidth, windowHeight);
    drawer.clear();

    // Create World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, -9.81f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Create Bodies
    body box = body(worldId, {halfWidth, halfHeight * 3 / 2}, 50.0f, 50.0f, b2_dynamicBody);
    drawer.addShape(box);

    body ground = body(worldId, {halfWidth,  5.0f}, 10.0f, 2 * windowWidth, b2_staticBody);
    drawer.addShape(ground);

    // Simulation loop
    const float timeStep = 1.0f / 30.0f;
    const int32_t subStepCount = 6;
    constexpr std::chrono::duration<double> DT{1.0 / 30.0};
    while (drawer.isOpen() && !g_stop) {
        std::optional<sf::Event> event = drawer.pollEvent();
        if (event && event->is<sf::Event::Closed>()) { drawer.close(); }
        
        b2World_Step(worldId, timeStep, subStepCount);
        drawer.drawAll();
    }

    return 0;
}
