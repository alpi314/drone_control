#pragma once

#include <vector>
#include <box2d/box2d.h>

class body {
private:
    b2ShapeId shapeId;
public:
    b2BodyId bodyId;
    body(b2WorldId worldId, b2Vec2 position, float height, float width, b2BodyType type = b2_staticBody, bool collide = true, float density = 1.0f); // constructor
    ~body() = default; // destructor
    b2Polygon getPolygon();
    std::vector<b2Vec2> getTransformedVertices();
};