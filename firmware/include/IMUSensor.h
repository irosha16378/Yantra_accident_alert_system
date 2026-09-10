#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

// MPU6050 Registers
constexpr uint8_t MPU6050_ADDR_DEFAULT = 0x68;
constexpr uint8_t MPU6050_REG_SMPLRT_DIV = 0x19;
constexpr uint8_t MPU6050_REG_CONFIG = 0x1A;
constexpr uint8_t MPU6050_REG_GYRO_CONFIG = 0x1B;
constexpr uint8_t MPU6050_REG_ACCEL_CONFIG = 0x1C;
constexpr uint8_t MPU6050_REG_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t MPU6050_REG_TEMP_OUT_H = 0x41;
constexpr uint8_t MPU6050_REG_GYRO_XOUT_H = 0x43;
constexpr uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
constexpr uint8_t MPU6050_REG_WHO_AM_I = 0x75;

struct IMUData {
    float accelX;      // Acceleration X (g)
    float accelY;      // Acceleration Y (g)
    float accelZ;      // Acceleration Z (g)
    float gyroX;       // Angular velocity X (deg/s)
    float gyroY;       // Angular velocity Y (deg/s)
    float gyroZ;       // Angular velocity Z (deg/s)
    float totalG;      // Vector magnitude |G| (g)
    float filteredG;   // Low-pass filtered vector magnitude (g)
    float pitch;       // Calculated Pitch angle (deg)
    float roll;        // Calculated Roll angle (deg)
    float temperature; // Chip temperature (°C)
};

class IMUSensor {
private:
    uint8_t _i2cAddr;
    bool _initialized;
    
    // Raw sensor readings
    int16_t _rawAccelX, _rawAccelY, _rawAccelZ;
    int16_t _rawGyroX, _rawGyroY, _rawGyroZ;
    int16_t _rawTemp;

    // Calibration offsets
    float _accelOffsetX, _accelOffsetY, _accelOffsetZ;
    float _gyroOffsetX, _gyroOffsetY, _gyroOffsetZ;

    // Filter alpha (0.0 to 1.0) for Exponential Moving Average
    float _filterAlpha;

    // Measured data
    IMUData _data;

    bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length);
    bool writeRegister(uint8_t reg, uint8_t value);

public:
    explicit IMUSensor(uint8_t i2cAddr = MPU6050_ADDR_DEFAULT);

    bool begin(int sdaPin = Config::PIN_SDA, int sclPin = Config::PIN_SCL);
    void update();
    void calibrate(uint16_t samples = 200);

    // Getters
    const IMUData& getData() const { return _data; }
    float getGX() const { return _data.accelX; }
    float getGY() const { return _data.accelY; }
    float getGZ() const { return _data.accelZ; }
    float getGyroX() const { return _data.gyroX; }
    float getGyroY() const { return _data.gyroY; }
    float getGyroZ() const { return _data.gyroZ; }
    float getTotalGForce() const { return _data.totalG; }
    float getFilteredGForce() const { return _data.filteredG; }
    float getPitch() const { return _data.pitch; }
    float getRoll() const { return _data.roll; }
    float getTemperature() const { return _data.temperature; }
    bool isInitialized() const { return _initialized; }

    // Detection utilities
    bool isImpactDetected(float thresholdG = 3.0f) const {
        return _data.totalG >= thresholdG;
    }

    bool isTumbleDetected(float pitchRollThresholdDeg = 60.0f) const {
        return (abs(_data.pitch) >= pitchRollThresholdDeg || abs(_data.roll) >= pitchRollThresholdDeg);
    }
};

#endif // IMU_SENSOR_H



