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
#include "include/drone.hpp"

volatile std::sig_atomic_t g_stop = 0;
extern "C" void sigint_handler(int) {
    g_stop = 1; // only set a sig_atomic_t
}

int main() {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);

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
    body droneBody = body(worldId, {halfWidth, halfHeight * 3 / 2}, 50.0f, 50.0f, b2_dynamicBody);
    drawer.addShape(droneBody);

    body ground = body(worldId, {halfWidth,  5.0f}, 10.0f, 2 * windowWidth, b2_staticBody);
    drawer.addShape(ground);

    // Create Drone
    std::vector<b2Vec2> motorLocal = { 
        { 25.0f, 40.0f }, { -25.0f, 40.0f }
    };
    std::vector<b2Vec2> motorDirections = {
        { 0.0f, 1.0f }, { 0.0f, 1.0f }
    };
    drone testDrone = drone(&droneBody, motorLocal, motorDirections);

    // Simulation loop (replace your previous loop)
    const float timeStep = 1.0f / 30.0f;
    const int32_t subStepCount = 6;

    while (drawer.isOpen() && !g_stop) {
        if (std::optional<sf::Event> event = drawer.pollEvent()) {
                if (event->getIf<sf::Event::Closed>()) {
                    drawer.close();
                    g_stop = 1;
                    break;
                }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    testDrone.applyThrustAll(1000000.0f); // both motors
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
                    testDrone.applyThrust(0, 1000000.0f); // left motor
                    testDrone.applyThrust(1, -1000000.0f); // right motor
                } else if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
                    testDrone.applyThrust(0, -1000000.0f); // left motor
                    testDrone.applyThrust(1, 1000000.0f); // right motor
                }
            }
        }

        // Advance physics
        b2World_Step(worldId, timeStep, subStepCount);

        // Render
        drawer.drawAll();
    }

    if (drawer.isOpen()) drawer.close();
    return 0;
}
