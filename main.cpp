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

    // Load drone texture
    sf::Texture droneTexture;
    std::string texturePath = "../../src/assets/drone.png";
    if (!droneTexture.loadFromFile(texturePath)) {
        // Try alternative path if running from project root
        texturePath = "src/assets/drone.png";
        if (!droneTexture.loadFromFile(texturePath)) {
            std::cerr << "Failed to load drone texture!" << std::endl;
            return 1;
        }
    }
    std::cout << "Loaded drone texture from: " << texturePath << std::endl;
    sf::Sprite droneSprite(droneTexture);
    
    // Get texture size and calculate physics body dimensions
    sf::Vector2u texSize = droneTexture.getSize();
    float droneScale = 0.15f; // Scale factor for the sprite
    float droneWidth = texSize.x * droneScale;
    float droneHeight = texSize.y * droneScale;
    
    // Set sprite origin to center
    droneSprite.setOrigin(sf::Vector2f(texSize.x / 2.0f, texSize.y / 2.0f));
    droneSprite.setScale(sf::Vector2f(droneScale, droneScale));
    
    std::cout << "Texture size: " << texSize.x << "x" << texSize.y << std::endl;
    std::cout << "Drone sprite dimensions: " << droneWidth << "x" << droneHeight << std::endl;

    // Create Bodies - match physics box to sprite dimensions
    // Ground surface is at ~15 (ground center 5 + half height 10), start drone just above it
    float groundSurface = 15.0f + droneHeight / 2.0f;
    
    // Calculate density to maintain similar mass to original 50x50 body
    // Original area = 50*50 = 2500, original density = 1.0, so original mass ~ 2500
    float originalArea = 50.0f * 50.0f;
    float newArea = droneWidth * droneHeight;
    float adjustedDensity = originalArea / newArea; // Keep mass similar
    
    body droneBody = body(worldId, {halfWidth, groundSurface}, droneHeight, droneWidth, b2_dynamicBody, true, adjustedDensity);
    // Don't add droneBody to drawer - we'll draw the sprite instead
    std::cout << "Drone body created at position: " << halfWidth << ", " << groundSurface << std::endl;
    std::cout << "Drone dimensions: " << droneWidth << "x" << droneHeight << ", density: " << adjustedDensity << std::endl;

    body ground = body(worldId, {halfWidth,  5.0f}, 10.0f, 2 * windowWidth, b2_staticBody);
    drawer.addShape(ground);
    std::cout << "Ground created at position: " << halfWidth << ", " << 5.0f << std::endl;

    // Create Drone - motor positions at the rotor locations (ends of the arms)
    // Rotors are at approximately 45% from center on each side based on image
    float motorOffsetX = droneWidth * 0.45f;
    float motorOffsetY = droneHeight * 0.3f; // Slightly above center where rotors are
    std::vector<b2Vec2> motorLocal = { 
        { -motorOffsetX, motorOffsetY }, { motorOffsetX, motorOffsetY }
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

    // Target altitude - start at ground level
    float targetAltitude = groundSurface;
    // add line shape to drawer to indicate target altitude
    body targetLine = body(worldId, {halfWidth, targetAltitude}, 2.0f, 2 * windowWidth, b2_kinematicBody, false);
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
                    b2Body_SetTransform(targetLine.bodyId, {halfWidth, targetAltitude}, b2MakeRot(0.0f));
                    std::cout << "Target Altitude: " << targetAltitude << std::endl;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                    targetAltitude -= 10.0f;
                    if (targetAltitude < 0.0f) targetAltitude = 0.0f;
                    b2Body_SetTransform(targetLine.bodyId, {halfWidth, targetAltitude}, b2MakeRot(0.0f));
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
        
        // Draw drone sprite at physics body position
        b2Vec2 dronePos = b2Body_GetPosition(droneBody.bodyId);
        b2Rot droneRot = b2Body_GetRotation(droneBody.bodyId);
        float droneAngle = b2Rot_GetAngle(droneRot);
        
        sf::Vector2f spritePos = drawer.toWindowLocation(dronePos.x, dronePos.y);
        droneSprite.setPosition(spritePos);
        droneSprite.setRotation(sf::degrees(-droneAngle * 180.0f / 3.14159f)); // Convert radians to degrees, negate for SFML
        drawer.getWindow().draw(droneSprite);
        
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
