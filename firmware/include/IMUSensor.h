#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>

/**
 * @brief Struct holding processed Gyroscope & Accelerometer sensor readings
 */
struct IMUData {
    // Processed Angular Rates (degrees/sec)
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;

    // Processed Accelerations (g-force units)
    float accelX = 0.0f;
    float accelY = 0.0f;
    float accelZ = 0.0f;

    // Derived Parameters
    float totalG = 0.0f;      // Magnitude: sqrt(ax^2 + ay^2 + az^2)
    float pitch  = 0.0f;      // Tilt angle around Y-axis (degrees)
    float roll   = 0.0f;      // Tilt angle around X-axis (degrees)

    // Raw Register Sensor Readings
    int16_t rawAccelX = 0;
    int16_t rawAccelY = 0;
    int16_t rawAccelZ = 0;
    int16_t rawGyroX  = 0;
    int16_t rawGyroY  = 0;
    int16_t rawGyroZ  = 0;

    // Calculated Calibration Offsets
    float gyroOffsetX = 0.0f;
    float gyroOffsetY = 0.0f;
    float gyroOffsetZ = 0.0f;
};

/**
 * @class IMUSensor
 * @brief I2C Driver for MPU6050 6-Axis Motion Tracking Device (Gyroscope + Accelerometer)
 */
class IMUSensor {
public:
    IMUSensor();

    /**
     * @brief Initialize I2C bus and wake up MPU6050 chip
     * @param sdaPin GPIO pin for I2C SDA (default: 21)
     * @param sclPin GPIO pin for I2C SCL (default: 22)
     * @param i2cAddr MPU6050 I2C device address (default: 0x68)
     * @return true if MPU6050 WHO_AM_I ID verified successfully, false otherwise
     */
    bool begin(uint8_t sdaPin = 21, uint8_t sclPin = 22, uint8_t i2cAddr = 0x68);

    /**
     * @brief Calibrate static Gyroscope zero-rate bias offsets
     * @param samples Number of readings to average (default: 200)
     */
    void calibrate(uint16_t samples = 200);

    /**
     * @brief Read raw sensor registers, apply calibration, and compute pitch/roll/rates
     */
    void update();

    /**
     * @brief Access latest IMU measurement data structure
     */
    const IMUData& getData() const { return data_; }

    /**
     * @brief Check if MPU6050 initialization succeeded
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief Impact detection evaluator
     */
    bool isImpactDetected(float thresholdG = 3.0f) const;

    /**
     * @brief Tumble tilt detection evaluator
     */
    bool isTumbleDetected(float thresholdDeg = 60.0f) const;

private:
    uint8_t address_ = 0x68;
    bool initialized_ = false;
    IMUData data_;

    // I2C Hardware Register Helpers
    bool writeRegister(uint8_t reg, uint8_t val);
    bool readRegisters(uint8_t reg, uint8_t* buffer, uint8_t length);
};

#endif // IMU_SENSOR_H

