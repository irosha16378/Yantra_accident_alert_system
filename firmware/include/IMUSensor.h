#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

class IMUSensor {
private:
    uint8_t _i2cAddr;
    bool _initialized;

    int16_t _rawAccelX;
    int16_t _rawAccelY;
    int16_t _rawAccelZ;

    float _gX;
    float _gY;
    float _gZ;
    float _totalGForce;

public:
    explicit IMUSensor(uint8_t i2cAddr = 0x68);

    bool begin();
    void update();

    float getGX() const { return _gX; }
    float getGY() const { return _gY; }
    float getGZ() const { return _gZ; }
    float getTotalGForce() const { return _totalGForce; }
    bool isInitialized() const { return _initialized; }
};

#endif // IMU_SENSOR_H


