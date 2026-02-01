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
#include "include/force_arrows.hpp"

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
        { -20.0f, 25.0f }, { 20.0f, 25.0f }
    };
    std::vector<b2Vec2> motorDirections = {
        { 0.0f, 1.0f }, { 0.0f, 1.0f }
    };
    drone testDrone = drone(&droneBody, motorLocal, motorDirections, 20000.0f);
    std::cout << "Drone created with " << motorLocal.size() << " motors, max thrust: " << testDrone.getMaxTotalThrust() << "N" << std::endl;

    // Create PID Controller
    bool enabled = true;
    float kp = 1.0f;
    float ki = 0.0005f;
    float kd = 200.0f;
    pid::hoverController hover = pid::hoverController(&testDrone, kp, ki, kd);
    std::cout << "PID Hover Controller created with Kp: " << kp << ", Ki: " << ki << ", Kd: " << kd << std::endl;

    // Target altitude
    float targetAltitude = 200.0f;
    // add line shape to drawer to indicate target altitude
    body targetLine = body(worldId, {0.0f, targetAltitude}, 2.0f, 2 * windowWidth, b2_kinematicBody, false);
    sf::Color red = sf::Color(255, 0, 0);
    drawer.addShape(targetLine, &red);

    // Create Force Arrow Drawer for visualizing forces
    ForceArrowDrawer forceDrawer(0.3f, 12.0f); // scale factor and arrow head size
    bool showForces = true; // Toggle with 'F' key

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
                if (keyPressed->scancode == sf::Keyboard::Scancode::F) {
                    showForces = !showForces;
                    std::cout << "Force Arrows " << (showForces ? "Enabled" : "Disabled") << std::endl;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    targetAltitude += 10.0f;
                    b2Body_SetTransform(targetLine.bodyId, {0.0f, targetAltitude}, b2MakeRot(0.0f));
                    std::cout << "Target Altitude: " << targetAltitude << std::endl;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    targetAltitude -= 10.0f;
                    if (targetAltitude < 0.0f) targetAltitude = 0.0f;
                    b2Body_SetTransform(targetLine.bodyId, {0.0f, targetAltitude}, b2MakeRot(0.0f));
                    std::cout << "Target Altitude: " << targetAltitude << std::endl;
                }
            }
        }

        if (enabled) {
            hover.update(targetAltitude, timeStep);
        }

        // Advance physics
        b2World_Step(worldId, timeStep, subStepCount);

        // Render
        drawer.clear();
        drawer.drawShapes();
        
        if (showForces) {
            forceDrawer.addNetForce(drawer.getWindow(),
                [&drawer](float x, float y) { return drawer.toWindowLocation(x, y); },
                testDrone, droneBody,
                sf::Color::Magenta,
                sf::Color::Cyan,
                sf::Color::Yellow);
        }
        
        drawer.display();
    }

    if (drawer.isOpen()) drawer.close();
    return 0;
}
