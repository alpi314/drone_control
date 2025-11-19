#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "../include/body.hpp"
#include "../include/draw.hpp"

draw::draw(unsigned int width, unsigned int height)
{
    backgroundColor = sf::Color::Black;
    shapeColor = sf::Color::White;
    window.create(sf::VideoMode({width, height}), "Drone Control");
}

draw::~draw()
{
    window.close();
}

void draw::clear()
{
    window.clear(backgroundColor);
}

void draw::addShape(body& shape)
{
    shapes.push_back(&shape);
}

sf::Vector2f draw::toWindowLocation(float x, float y) {
    return sf::Vector2f(x, window.getSize().y - y);
}

sf::ConvexShape draw::convexShape(const std::vector<b2Vec2>& shape, sf::Color color) {
    sf::ConvexShape polygon;
    polygon.setPointCount(shape.size());
    for (int i = 0; i < shape.size(); ++i) {
        polygon.setPoint(i, toWindowLocation(shape[i].x, shape[i].y));
    }
    polygon.setFillColor(color);
    return polygon;
}

void draw::drawAll() {
    draw::clear();
    for (const auto& shape : shapes) {
        std::vector<b2Vec2> polygonData = shape->getTransformedVertices();
        sf::ConvexShape polygon = convexShape(polygonData, shapeColor);
        window.draw(polygon);
    }
    window.display();
}

void draw::close() {
    window.close();
}

bool draw::isOpen() {
    return window.isOpen();
}

std::optional<sf::Event> draw::pollEvent() {
    return window.pollEvent();
}