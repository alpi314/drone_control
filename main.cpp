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
#include <optional>
#include <string>

#include "include/draw.hpp"
#include "include/body.hpp"
#include "include/drone.hpp"
#include "include/controller/pid.hpp"

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
    std::cout << "Window created: " << windowWidth << "x" << windowHeight << std::endl;

    // Create World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, -9.81f};
    b2WorldId worldId = b2CreateWorld(&worldDef);
    std::cout << "Box2D World created with gravity: " << worldDef.gravity.x << ", " << worldDef.gravity.y << std::endl;

    // Create Bodies
    body droneBody = body(worldId, {halfWidth, 100.0f}, 50.0f, 50.0f, b2_dynamicBody);
    drawer.addShape(droneBody);
    std::cout << "Drone body created at position: " << halfWidth << ", " << 20.0f << std::endl;

    body ground = body(worldId, {halfWidth,  5.0f}, 10.0f, 2 * windowWidth, b2_staticBody);
    drawer.addShape(ground);
    std::cout << "Ground created at position: " << halfWidth << ", " << 5.0f << std::endl;

    // Create Drone
    std::vector<b2Vec2> motorLocal = { 
        { -12.5f, 40.0f }, { 12.5f, 40.0f }
    };
    std::vector<b2Vec2> motorDirections = {
        { 0.0f, 1.0f }, { 0.0f, 1.0f }
    };
    drone testDrone = drone(&droneBody, motorLocal, motorDirections);
    std::cout << "Drone created with " << motorLocal.size() << " motors." << std::endl;

    // Create PID Controller
    bool enabled = true;
    float kp = 1.5f; // Proportional gain
    float ki = 0.0001f;  // Integral gain
    float kd = 1.0f; // Derivative gain
    pid::hoverController hover = pid::hoverController(&testDrone, kp, ki, kd);
    std::cout << "PID Hover Controller created with Kp: " << kp << ", Ki: " << ki << ", Kd: " << kd << std::endl;

    // Simulation loop (replace your previous loop)
    const float timeStep = 1.0f / 30.0f;
    const int32_t subStepCount = 6;
    while (drawer.isOpen() && !g_stop) {
        if (std::optional<sf::Event> event = drawer.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                drawer.close();
                g_stop = 1;
                break;
            }

            if (const auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    enabled = !enabled;
                    std::cout << "PID Controller " << (enabled ? "Enabled" : "Disabled") << std::endl;
                }
            }
        }

        if (enabled) {
            hover.update(200.0f, timeStep);
        }

        // Advance physics
        b2World_Step(worldId, timeStep, subStepCount);

        // Render
        drawer.drawAll();
    }

    if (drawer.isOpen()) drawer.close();
    return 0;
}
