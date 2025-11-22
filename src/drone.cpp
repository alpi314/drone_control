#include "../include/drone.hpp"

b2Vec2 rotateVector(const b2Vec2& vec, const b2Rot& rot) {
    return {
        rot.c * vec.x - rot.s * vec.y,
        rot.s * vec.x + rot.c * vec.y
    };
}

drone::drone(body* droneBody, std::vector<b2Vec2> motorPositions, std::vector<b2Vec2> motorDirections) {
    this->droneBody = droneBody;
    this->motorPositions = motorPositions;
    this->motorDirections = motorDirections;
}

void drone::applyThrust(int motor, b2Vec2 thrustLocation, b2Vec2 thrustVec) {
    b2Body_ApplyForce(droneBody->bodyId, thrustVec, thrustLocation, true);
}

void drone::applyThrust(int motor, float thrust) {
    b2Vec2 pos = b2Body_GetPosition(droneBody->bodyId);
    b2Rot rot = b2Body_GetRotation(droneBody->bodyId);

    b2Vec2 thrustLocation = pos + rotateVector(motorPositions[motor], rot);
    b2Vec2 thrustDirection = rotateVector(motorDirections[motor], rot);
    b2Vec2 thrustVec = thrustDirection * thrust;

    applyThrust(motor, thrustLocation, thrustVec);
}

void drone::applyThrustAll(float thrust) {
    for (size_t i = 0; i < motorPositions.size(); ++i) {
        applyThrust(i, thrust);
    }
}