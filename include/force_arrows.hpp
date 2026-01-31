#pragma once

#include <vector>
#include <functional>
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "drone.hpp"

class ForceArrowDrawer {
private:
    float scaleFactor;
    float arrowHeadSize;
    
    void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, sf::Color color);

public:
    ForceArrowDrawer(float scaleFactor = 0.5f, float arrowHeadSize = 10.0f);
    
    void addNetForce(sf::RenderWindow& window,
                     std::function<sf::Vector2f(float, float)> toWindowLocation,
                     drone& d, body& droneBody,
                     sf::Color sumColor = sf::Color::White,
                     sf::Color vertColor = sf::Color::Green,
                     sf::Color horizColor = sf::Color::Red);
    
    void setScaleFactor(float scale) { scaleFactor = scale; }
    void setArrowHeadSize(float size) { arrowHeadSize = size; }
};
