#include <box2d/box2d.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

int main() {
    std::cout << "Box2D C API Example\n";

    b2WorldDef worldDef = {};
    worldDef.gravity = {0.0f, -9.81f};

    b2WorldId world = b2CreateWorld(&worldDef);
    if (!b2World_IsValid(world)) {
        std::cerr << "Failed to create world\n";
        return 1;
    }
    std::cout << "World created successfully\n";

    b2BodyDef bodyDef = {};
    b2BodyId body = b2CreateBody(world, &bodyDef);
    if (!b2Body_IsValid(body)) {
        std::cerr << "Failed to create body\n";
        b2DestroyWorld(world);
        return 1;
    }
    std::cout << "Body created successfully\n";

    b2Polygon poly = {};
    poly.count = 4;
    poly.vertices[0] = { -1.0f, -1.0f };
    poly.vertices[1] = {  1.0f, -1.0f };
    poly.vertices[2] = {  1.0f,  1.0f };
    poly.vertices[3] = { -1.0f,  1.0f };

    b2ShapeDef shapeDef = {};
    shapeDef.density = 1.0f;

    b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &poly);
    if (!b2Shape_IsValid(shape)) {
        std::cerr << "Failed to create shape\n";
        b2DestroyWorld(world);
        return 1;
    }
    std::cout << "Shape created successfully\n";

    b2Body_ApplyMassFromShapes(body);

    // Simulation Loop 
    const float timeStep = 1.0f / 60.0f;
    const int subSteps = 1;
    for (int i = 0; i < 600; ++i) {
        std::cout << "Step " << i << "\n";
        b2World_Step(world, timeStep, subSteps);

        b2Vec2 pos = b2Body_GetPosition(body);
        std::cout << "Position at step " << i << ": (" << pos.x << ", " << pos.y << ")\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    b2DestroyWorld(world);
    return 0;
}
