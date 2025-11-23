#pragma once

#include "../drone.hpp"

namespace pid {

class  hoverController {
private:
    drone* controlledDrone;
    float kp; // Proportional gain
    float ki; // Integral gain
    float kd; // Derivative gain
    float integralError;
    float previousError;
    float proportional(float error);
    float integral(float error, float deltaTime);
    float derivative(float error, float deltaTime);
    float computeControl(float error, float deltaTime);
public:
    hoverController(drone* controlledDrone, float kp, float ki, float kd);
    void update(float targetAltitude, float deltaTime);
};

   
}