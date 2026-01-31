

#include "../include/drone.hpp"
#include <box2d/box2d.h>
#include <iostream>
#include <algorithm>

b2Vec2 rotateVector(const b2Vec2& vec, const b2Rot& rot) {
    return {
        rot.c * vec.x - rot.s * vec.y,
        rot.s * vec.x + rot.c * vec.y
    };
}

drone::drone(body* droneBody, std::vector<b2Vec2> motorPositions, std::vector<b2Vec2> motorDirections, float maxThrustPerMotor) {
    this->droneBody = droneBody;
    this->motorPositions = motorPositions;
    this->motorDirections = motorDirections;
    this->maxThrustPerMotor = maxThrustPerMotor;
    this->lastThrustValues.resize(motorPositions.size(), 0.0f);
}

float drone::altitude() {
    b2Vec2 position = b2Body_GetPosition(droneBody->bodyId);
    return position.y;
}

float drone::gravitationalForce() {
    b2MassData mass = b2Body_GetMassData(droneBody->bodyId);
    const float gravity = 9.81f; // m/s^2
    return mass.mass * gravity;
}

void drone::applyThrust(int motor, b2Vec2 thrustLocation, b2Vec2 thrustVec) {
    b2Body_ApplyForce(droneBody->bodyId, thrustVec, thrustLocation, true);
}

void drone::applyThrust(int motor, float thrust) {
    thrust = std::clamp(thrust, 0.0f, maxThrustPerMotor);
    
    b2Vec2 pos = b2Body_GetPosition(droneBody->bodyId);
    b2Rot rot = b2Body_GetRotation(droneBody->bodyId);

    b2Vec2 thrustLocation = pos + rotateVector(motorPositions[motor], rot);
    b2Vec2 thrustDirection = rotateVector(motorDirections[motor], rot);
    b2Vec2 thrustVec = thrustDirection * thrust;

    lastThrustValues[motor] = thrust;

    applyThrust(motor, thrustLocation, thrustVec);
}

void drone::applyThrustEvenly(float thrust) {
    size_t n_motors = motorPositions.size();
    for (size_t i = 0; i < n_motors; ++i) {
        applyThrust(i, thrust / n_motors);
    }
}