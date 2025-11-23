#include "../../include/controller/pid.hpp"
#include  "../../include/drone.hpp"
#include <iostream>

using namespace pid;

hoverController::hoverController(drone* controlledDrone, float kp, float ki, float kd) {
    this->controlledDrone = controlledDrone;
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->integralError = 0.0f;
    this->previousError = 0.0f;
}

float hoverController::proportional(float error) {
    std::cout << "Proportional Error: " << error << std::endl;
    return kp * error;
}

float hoverController::integral(float error, float deltaTime) {
    integralError += error * deltaTime;
    std::cout << "Integral Error: " << integralError << std::endl;
    return ki * integralError;
}

float hoverController::derivative(float error, float deltaTime) {
    float derivativeError = (error - previousError) / deltaTime;
    previousError = error;
    std::cout << "Derivative Error: " << derivativeError << std::endl;
    return kd * derivativeError;
}
    
float hoverController::computeControl(float error, float deltaTime) {
    float p = proportional(error);
    float i = integral(error, deltaTime);
    float d = derivative(error, deltaTime);
    return p + i + d;
}
    
void hoverController::update(float targetAltitude, float deltaTime) {
    float currentAltitude = controlledDrone->altitude();
    std::cout << "Current Altitude: " << currentAltitude << std::endl;
    float error = targetAltitude - currentAltitude;
    std::cout << "Error: " << error << std::endl;
    float controlOutput = computeControl(error, deltaTime);
    float gravityCompensation = controlledDrone->gravitationalForce();
    float totalThrust = controlOutput + gravityCompensation;
    controlledDrone->applyThrustEvenly(totalThrust);
}
