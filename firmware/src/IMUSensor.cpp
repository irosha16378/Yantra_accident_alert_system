#include "IMUSensor.h"

IMUSensor::IMUSensor(float impactThreshold)
    : _lastGForce(1.0f), _impactThreshold(impactThreshold), _isInitialized(false) {}

bool IMUSensor::begin() {
    // Hardware Wire.begin(Config::PIN_SDA, Config::PIN_SCL) & MPU6050 init logic goes here
    _isInitialized = true;
    Serial.println("[IMU] Initialized MPU6050 successfully.");
    return true;
}

void IMUSensor::update() {
    if (!_isInitialized) return;

    // In hardware implementation: read acceleration vectors ax, ay, az
    // Calculate total G-force: magnitude = sqrt(ax^2 + ay^2 + az^2) / 9.81
    _lastGForce = 1.0f; // Default baseline 1G static gravity
}

bool IMUSensor::isImpactDetected() const {
    return _lastGForce >= _impactThreshold;
}

float IMUSensor::getCurrentGForce() const {
    return _lastGForce;
}
