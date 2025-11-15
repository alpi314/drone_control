// main.cpp
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

static constexpr float PPM = 30.0f; // pixels per meter
static constexpr float RAD2DEG = 180.0f / 3.14159265358979323846f;

static inline sf::Vector2f worldToScreen(const b2Vec2& v, unsigned winH) {
    return sf::Vector2f(v.x * PPM, float(winH) - v.y * PPM); // flip Y for screen coords
}

int main() {
    const unsigned WIN_W = 800, WIN_H = 600;
    unsigned int bitsPerPixel = 32;
    sf::Vector2u windowSize(WIN_W, WIN_H);
    sf::VideoMode modeSize(windowSize, bitsPerPixel);
    sf::RenderWindow window(modeSize, "Box2D C API + SFML Example");
    window.setFramerateLimit(60);

    std::cout << "Box2D C API + SFML Example\n";

    // --- create world (C API) ---
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = { 0.0f, -9.81f };
    b2WorldId world = b2CreateWorld(&worldDef);
    if (!b2World_IsValid(world)) { std::cerr << "Failed create world\n"; return 1; }

    // --- dynamic body ---
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = { 5.0f, 10.0f };                // meters
    bodyDef.type = b2BodyType::b2_dynamicBody;
    b2BodyId body = b2CreateBody(world, &bodyDef);
    if (!b2Body_IsValid(body)) { std::cerr << "Failed create body\n"; b2DestroyWorld(world); return 1; }

    // polygon vertices (local body coords) - centered 2x2 box
    b2Polygon poly = {};
    poly.count = 4;
    poly.vertices[0] = { -1.0f, -1.0f };
    poly.vertices[1] = {  1.0f, -1.0f };
    poly.vertices[2] = {  1.0f,  1.0f };
    poly.vertices[3] = { -1.0f,  1.0f };

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;

    b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &poly);
    if (!b2Shape_IsValid(shape)) { std::cerr << "Failed create shape\n"; b2DestroyWorld(world); return 1; }
    b2Body_ApplyMassFromShapes(body);

    // compute box size in meters from polygon (robust if polygon changes)
    float minx = poly.vertices[0].x, maxx = poly.vertices[0].x;
    float miny = poly.vertices[0].y, maxy = poly.vertices[0].y;
    for (int i = 1; i < poly.count; ++i) {
        minx = std::min(minx, poly.vertices[i].x);
        maxx = std::max(maxx, poly.vertices[i].x);
        miny = std::min(miny, poly.vertices[i].y);
        maxy = std::max(maxy, poly.vertices[i].y);
    }
    const float box_w_m = maxx - minx;
    const float box_h_m = maxy - miny;

    // SFML rectangle to represent the polygon (size in pixels)
    sf::RectangleShape rect(sf::Vector2f(box_w_m * PPM, box_h_m * PPM));
    rect.setOrigin(rect.getSize() * 0.5f);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(2.0f);
    rect.setOutlineColor(sf::Color::White);

    // --- ground (static) ---
    b2BodyDef gdef = b2DefaultBodyDef();
    gdef.position = { 0.0f, 1.0f };
    b2BodyId ground = b2CreateBody(world, &gdef);
    b2Polygon gpoly = {};
    gpoly.count = 4;
    gpoly.vertices[0] = { -50.0f, -1.0f };
    gpoly.vertices[1] = {  50.0f, -1.0f };
    gpoly.vertices[2] = {  50.0f,  1.0f };
    gpoly.vertices[3] = { -50.0f,  1.0f };
    b2ShapeDef gshape = b2DefaultShapeDef();
    gshape.density = 0.0f;
    b2CreatePolygonShape(ground, &gshape, &gpoly);
    b2Body_ApplyMassFromShapes(ground);

    // ground visual (line)
    sf::Vertex v1;
    v1.position = worldToScreen({ -50.0f, 1.0f }, WIN_H);
    sf::Vertex v2;
    v2.position = worldToScreen({  50.0f, 1.0f }, WIN_H);
    sf::Vertex groundLine[] = { v1, v2 };

    // simulation parameters
    const float timeStep = 1.0f / 60.0f;
    const int subSteps = 1;

    // main loop
    while (window.isOpen()) {
        std::optional<sf::Event> ev;
        while (ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // step physics
        b2World_Step(world, timeStep, subSteps);

        // get body transform
        b2Vec2 pos = b2Body_GetPosition(body);      // meters
        b2Rot angle = b2Body_GetRotation(body);        // radians
        float angle_f = std::atan2(angle.s, angle.c);
        float angle_deg = angle_f * RAD2DEG;

        // convert and apply to SFML rect
        sf::Vector2f screenPos = worldToScreen(pos, WIN_H);
        rect.setPosition(screenPos);
        float sf_angle_deg = -angle_f * RAD2DEG; // negate because screen y is flipped
        sf::Angle sf_angle = sf::degrees(sf_angle_deg);
        rect.setRotation(sf_angle);

        // render
        window.clear(sf::Color(20, 20, 20));
        window.draw(rect);
        window.draw(groundLine, 2, sf::PrimitiveType::Lines);
        window.display();

        // tiny sleep to avoid burning CPU (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    b2DestroyWorld(world);
    return 0;
}
