#include "HandlebarSensor.h"

HandlebarSensor::HandlebarSensor(uint8_t pinLeft, uint8_t pinRight)
    : _pinLeft(pinLeft), _pinRight(pinRight) {}

void HandlebarSensor::begin() {
    pinMode(_pinLeft, INPUT_PULLUP);
    pinMode(_pinRight, INPUT_PULLUP);
    Serial.println("[Handlebar] Dual handlebar sensors initialized.");
}

bool HandlebarSensor::isLeftHandOn() const {
    return digitalRead(_pinLeft) == LOW; // Active LOW touch sensor
}

bool HandlebarSensor::isRightHandOn() const {
    return digitalRead(_pinRight) == LOW; // Active LOW touch sensor
}

bool HandlebarSensor::areBothHandsOff() const {
    return !isLeftHandOn() && !isRightHandOn();
}
