#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "../include/body.hpp"
#include "../include/draw.hpp"

draw::draw()
{
    window.create(sf::VideoMode({800, 600}), "Drone Control");
}

draw::~draw()
{
    window.close();
}

void draw::clear()
{
    window.clear(sf::Color::Black);
}

void draw::addShape(body& shape)
{
    shapes.push_back(&shape);
}

sf::Vector2f draw::toWindowLocation(float x, float y) {
    return sf::Vector2f(x, window.getSize().y - y);
}

sf::ConvexShape draw::convexShape(const b2Polygon& shape, sf::Color color) {
    sf::ConvexShape polygon;
    polygon.setPointCount(shape.count);
    for (int i = 0; i < shape.count; ++i) {
        polygon.setPoint(i, toWindowLocation(shape.vertices[i].x, shape.vertices[i].y));
    }
    polygon.setFillColor(color);
    return polygon;
}

void draw::drawAll() {
    for (const auto& shape : shapes) {
        b2Polygon polygonData = shape->getPolygon();
        sf::ConvexShape polygon = convexShape(polygonData, shapeColor);
        window.draw(polygon);
    }
    window.display();
}