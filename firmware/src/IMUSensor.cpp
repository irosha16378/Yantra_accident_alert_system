#include "IMUSensor.h"
#include <math.h>

IMUSensor::IMUSensor(uint8_t i2cAddr)
    : _i2cAddr(i2cAddr),
      _initialized(false),
      _rawAccelX(0), _rawAccelY(0), _rawAccelZ(0),
      _gX(0.0f), _gY(0.0f), _gZ(1.0f),
      _totalGForce(1.0f) {}

bool IMUSensor::begin() {
    Wire.begin(Config::PIN_SDA, Config::PIN_SCL);
    delay(100);

    // Wake up MPU6050 (Power Management Register 0x6B -> write 0x00)
    Wire.beginTransmission(_i2cAddr);
    Wire.write(0x6B);
    Wire.write(0x00);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
        _initialized = true;
        Serial.print("[IMU] MPU6050 detected at I2C Address 0x");
        Serial.println(_i2cAddr, HEX);
        return true;
    } else {
        _initialized = false;
        Serial.print("[IMU ERROR] MPU6050 not found at address 0x");
        Serial.println(_i2cAddr, HEX);
        return false;
    }
}

void IMUSensor::update() {
    if (!_initialized) return;

    // Request 6 bytes from MPU6050 starting at ACCEL_XOUT_H (0x3B)
    Wire.beginTransmission(_i2cAddr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(_i2cAddr, (uint8_t)6, (uint8_t)true);

    if (Wire.available() >= 6) {
        _rawAccelX = (Wire.read() << 8) | Wire.read();
        _rawAccelY = (Wire.read() << 8) | Wire.read();
        _rawAccelZ = (Wire.read() << 8) | Wire.read();

        // Standard MPU6050 Sensitivity Scale Factor for ±2g is 16384 LSB/g
        _gX = (float)_rawAccelX / 16384.0f;
        _gY = (float)_rawAccelY / 16384.0f;
        _gZ = (float)_rawAccelZ / 16384.0f;

        // Calculate vector magnitude (Total G-Force)
        _totalGForce = sqrtf(_gX * _gX + _gY * _gY + _gZ * _gZ);
    }
}


