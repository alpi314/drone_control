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

void draw::addShape(body& shape, sf::Color* color)
{
    shapes.push_back(&shape);
    if (color) {
        shapeColors.push_back(*color);
    } else {
        shapeColors.push_back(shapeColor);
    }
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

void draw::drawShapes() {
    for (size_t i = 0; i < shapes.size(); ++i) {
        std::vector<b2Vec2> polygonData = shapes[i]->getTransformedVertices();
        sf::ConvexShape polygon = convexShape(polygonData, shapeColors[i]);
        window.draw(polygon);
    }
}

void draw::display() {
    window.display();
}

void draw::drawAll() {
    draw::clear();
    drawShapes();
    display();
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