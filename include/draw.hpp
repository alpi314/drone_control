#pragma once

#include <vector>
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "body.hpp"

class draw
{
private:
    sf::RenderWindow window;

    sf::Color backgroundColor;
    sf::Color shapeColor;
    std::vector<body*> shapes;
public:
    draw(); // constructor
    ~draw(); // destructor
    void clear();
    void addShape(body& shape);
    void drawAll();
    sf::ConvexShape convexShape(const b2Polygon& shape, sf::Color color);
    sf::Vector2f toWindowLocation(float x, float y);
};