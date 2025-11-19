#include <box2d/box2d.h>
#include <vector>
#include "../include/body.hpp"

body::body(b2WorldId worldId, b2Vec2 position, float height, float width, b2BodyType type) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = position;
    bodyDef.type = type;
    bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon polygon = b2MakeBox(width / 2, height / 2);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
}

b2Polygon body::getPolygon() {
    return b2Shape_GetPolygon(shapeId);;
}

std::vector<b2Vec2> body::getTransformedVertices() {
    b2Polygon polygon = getPolygon();
    std::vector<b2Vec2> transformedVertices;
    
    for (int i = 0; i < polygon.count; ++i) {
        b2Vec2 localVertex = polygon.vertices[i];
        b2Vec2 worldVertex = b2Body_GetWorldPoint(bodyId, localVertex);
        transformedVertices.push_back(worldVertex);
    }
    return transformedVertices;
}