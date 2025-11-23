#pragma once

#include "body.hpp"
#include <vector>

class drone {
    private:
        body* droneBody;
        std::vector<b2Vec2> motorPositions;
        std::vector<b2Vec2> motorDirections;
        void applyThrust(int motor, b2Vec2 thrustLocation, b2Vec2 thrustVec);
    public:
        drone(body* droneBody, std::vector<b2Vec2> motorPositions, std::vector<b2Vec2> motorDirections);
        void applyThrust(int motor, float thrust);
        void applyThrustEvenly(float thrust);
        float altitude();
        float gravitationalForce();
};
