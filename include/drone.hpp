#pragma once

#include "body.hpp"
#include <vector>

class drone {
    private:
        body* droneBody;
        std::vector<b2Vec2> motorPositions;
        std::vector<b2Vec2> motorDirections;
        std::vector<float> lastThrustValues;
        float maxThrustPerMotor;
        void applyThrust(int motor, b2Vec2 thrustLocation, b2Vec2 thrustVec);
    public:
        drone(body* droneBody, std::vector<b2Vec2> motorPositions, std::vector<b2Vec2> motorDirections, float maxThrustPerMotor = 1000.0f);
        void applyThrust(int motor, float thrust);
        void applyThrustEvenly(float thrust);
        float altitude();
        float gravitationalForce();
        float getMaxTotalThrust() const { return maxThrustPerMotor * motorPositions.size(); }
        
        body* getBody() const { return droneBody; }
        const std::vector<b2Vec2>& getMotorPositions() const { return motorPositions; }
        const std::vector<b2Vec2>& getMotorDirections() const { return motorDirections; }
        const std::vector<float>& getLastThrustValues() const { return lastThrustValues; }
};
