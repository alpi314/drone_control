#include "../include/force_arrows.hpp"
#include <cmath>
#include <functional>

static b2Vec2 rotateVector(const b2Vec2& vec, const b2Rot& rot) {
    return {
        rot.c * vec.x - rot.s * vec.y,
        rot.s * vec.x + rot.c * vec.y
    };
}

ForceArrowDrawer::ForceArrowDrawer(float scaleFactor, float arrowHeadSize)
    : scaleFactor(scaleFactor), arrowHeadSize(arrowHeadSize) {
}

void ForceArrowDrawer::drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, sf::Color color) {
    sf::Vector2f dir = end - start;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 5.0f) return;
    
    dir /= len;
    sf::Vector2f perp = { -dir.y, dir.x };
    
    float lineThickness = 2.0f;
    sf::RectangleShape line(sf::Vector2f(len - arrowHeadSize, lineThickness));
    line.setFillColor(color);
    line.setOrigin(sf::Vector2f(0.f, lineThickness / 2.f));
    line.setPosition(start);
    line.setRotation(sf::radians(std::atan2(dir.y, dir.x)));
    window.draw(line);
    
    sf::ConvexShape head(3);
    sf::Vector2f base = end - dir * arrowHeadSize;
    head.setPoint(0, end);
    head.setPoint(1, base + perp * (arrowHeadSize * 0.5f));
    head.setPoint(2, base - perp * (arrowHeadSize * 0.5f));
    head.setFillColor(color);
    window.draw(head);
}

void ForceArrowDrawer::addNetForce(sf::RenderWindow& window,
                                    std::function<sf::Vector2f(float, float)> toWindowLocation,
                                    drone& d, body& droneBody,
                                    sf::Color sumColor,
                                    sf::Color vertColor,
                                    sf::Color horizColor) {
    b2Vec2 pos = b2Body_GetPosition(droneBody.bodyId);
    b2Rot rot = b2Body_GetRotation(droneBody.bodyId);
    
    // Gravity
    float gravity = d.gravitationalForce();
    b2Vec2 netForce = { 0.0f, -gravity };
    
    // Sum all thrusts
    const auto& thrustValues = d.getLastThrustValues();
    const auto& motorPositions = d.getMotorPositions();
    const auto& motorDirections = d.getMotorDirections();
    
    for (size_t i = 0; i < thrustValues.size() && i < motorDirections.size(); ++i) {
        b2Vec2 thrustDir = rotateVector(motorDirections[i], rot);
        netForce.x += thrustDir.x * thrustValues[i];
        netForce.y += thrustDir.y * thrustValues[i];
    }
    
    sf::Vector2f startPos = toWindowLocation(pos.x, pos.y);
    
    // Vertical component (green)
    if (std::abs(netForce.y) > 0.1f) {
        float vertLen = std::clamp(std::abs(netForce.y) * scaleFactor, 10.0f, 150.0f);
        float endY = pos.y + (netForce.y > 0 ? vertLen : -vertLen);
        sf::Vector2f vertEnd = toWindowLocation(pos.x, endY);
        drawArrow(window, startPos, vertEnd, vertColor);
    }
    
    // Horizontal component (red)
    if (std::abs(netForce.x) > 0.1f) {
        float horizLen = std::clamp(std::abs(netForce.x) * scaleFactor, 10.0f, 150.0f);
        float endX = pos.x + (netForce.x > 0 ? horizLen : -horizLen);
        sf::Vector2f horizEnd = toWindowLocation(endX, pos.y);
        drawArrow(window, startPos, horizEnd, horizColor);
    }
    
    // Net force (white)
    float mag = std::sqrt(netForce.x * netForce.x + netForce.y * netForce.y);
    if (mag > 0.1f) {
        float arrowLen = std::clamp(mag * scaleFactor, 10.0f, 150.0f);
        b2Vec2 dir = { netForce.x / mag, netForce.y / mag };
        float endX = pos.x + dir.x * arrowLen;
        float endY = pos.y + dir.y * arrowLen;
        sf::Vector2f netEnd = toWindowLocation(endX, endY);
        drawArrow(window, startPos, netEnd, sumColor);
    }
}
