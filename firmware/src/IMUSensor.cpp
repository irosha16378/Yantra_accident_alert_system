#include "IMUSensor.h"
#include <Wire.h>
#include <cmath>

// MPU6050 Register Map Definitions
namespace MPU6050Register {
    constexpr uint8_t SMPLRT_DIV   = 0x19;
    constexpr uint8_t CONFIG       = 0x1A;
    constexpr uint8_t GYRO_CONFIG  = 0x1B;
    constexpr uint8_t ACCEL_CONFIG = 0x1C;
    constexpr uint8_t ACCEL_XOUT_H = 0x3B;
    constexpr uint8_t PWR_MGMT_1   = 0x6B;
    constexpr uint8_t WHO_AM_I     = 0x75;
}

// Sensitivity Scale Factors
constexpr float GYRO_SCALE_250DPS = 131.0f;  // 131 LSB / (deg/s) for +/-250 dps
constexpr float ACCEL_SCALE_8G    = 4096.0f;  // 4096 LSB / g for +/-8g

IMUSensor::IMUSensor() {
}

bool IMUSensor::begin(uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddr) {
    address_ = i2cAddr;
    initialized_ = false;

    // Initialize I2C Bus on specified ESP32 pins
    Wire.begin(sdaPin, sclPin);
    Wire.setClock(400000); // 400kHz Fast I2C mode

    delay(10);

    // Read WHO_AM_I register (0x75) to verify MPU6050 communication
    uint8_t whoAmI = 0;
    if (!readRegisters(MPU6050Register::WHO_AM_I, &whoAmI, 1)) {
        Serial.println("[IMU Error] Failed to read WHO_AM_I register.");
        return false;
    }

    if (whoAmI != 0x68 && whoAmI != 0x69 && whoAmI != 0x70) {
        Serial.printf("[IMU Error] Unexpected WHO_AM_I ID: 0x%02X (Expected 0x68)\n", whoAmI);
        return false;
    }

    Serial.printf("[IMU Success] MPU6050 found! WHO_AM_I = 0x%02X\n", whoAmI);

    // Reset PWR_MGMT_1 (0x6B) to 0x00 to wake MPU6050 from sleep mode
    if (!writeRegister(MPU6050Register::PWR_MGMT_1, 0x00)) {
        Serial.println("[IMU Error] Failed to wake MPU6050.");
        return false;
    }
    delay(10);

    // Set Gyroscope Full Scale Range to +/- 250 deg/s (GYRO_CONFIG = 0x00)
    writeRegister(MPU6050Register::GYRO_CONFIG, 0x00);

    // Set Accelerometer Full Scale Range to +/- 8g (ACCEL_CONFIG = 0x10)
    writeRegister(MPU6050Register::ACCEL_CONFIG, 0x10);

    initialized_ = true;
    return true;
}

void IMUSensor::calibrate(uint16_t samples) {
    if (!initialized_) {
        Serial.println("[IMU Warning] Cannot calibrate; device not initialized.");
        return;
    }

    Serial.printf("[IMU Calibration] Measuring static zero-rate Gyroscope offsets (%u samples)...\n", samples);
    int32_t sumGX = 0, sumGY = 0, sumGZ = 0;

    for (uint16_t i = 0; i < samples; i++) {
        uint8_t buffer[14];
        if (readRegisters(MPU6050Register::ACCEL_XOUT_H, buffer, 14)) {
            int16_t rawGX = (int16_t)((buffer[8] << 8) | buffer[9]);
            int16_t rawGY = (int16_t)((buffer[10] << 8) | buffer[11]);
            int16_t rawGZ = (int16_t)((buffer[12] << 8) | buffer[13]);

            sumGX += rawGX;
            sumGY += rawGY;
            sumGZ += rawGZ;
        }
        delay(3);
    }

    data_.gyroOffsetX = ((float)sumGX / samples) / GYRO_SCALE_250DPS;
    data_.gyroOffsetY = ((float)sumGY / samples) / GYRO_SCALE_250DPS;
    data_.gyroOffsetZ = ((float)sumGZ / samples) / GYRO_SCALE_250DPS;

    Serial.printf("[IMU Calibration Complete] Gyro Offsets -> X: %.2f deg/s | Y: %.2f deg/s | Z: %.2f deg/s\n",
                  data_.gyroOffsetX, data_.gyroOffsetY, data_.gyroOffsetZ);
}

void IMUSensor::update() {
    if (!initialized_) return;

    // Read 14 consecutive bytes starting from ACCEL_XOUT_H (0x3B)
    // 0..5: Accel X/Y/Z, 6..7: Temp, 8..13: Gyro X/Y/Z
    uint8_t buffer[14];
    if (!readRegisters(MPU6050Register::ACCEL_XOUT_H, buffer, 14)) {
        return;
    }

    // Parse Big-End 16-bit integers
    data_.rawAccelX = (int16_t)((buffer[0] << 8) | buffer[1]);
    data_.rawAccelY = (int16_t)((buffer[2] << 8) | buffer[3]);
    data_.rawAccelZ = (int16_t)((buffer[4] << 8) | buffer[5]);

    data_.rawGyroX  = (int16_t)((buffer[8] << 8) | buffer[9]);
    data_.rawGyroY  = (int16_t)((buffer[10] << 8) | buffer[11]);
    data_.rawGyroZ  = (int16_t)((buffer[12] << 8) | buffer[13]);

    // Convert Accelerometer to g-force units (+/- 8g range)
    data_.accelX = (float)data_.rawAccelX / ACCEL_SCALE_8G;
    data_.accelY = (float)data_.rawAccelY / ACCEL_SCALE_8G;
    data_.accelZ = (float)data_.rawAccelZ / ACCEL_SCALE_8G;

    // Convert Gyroscope to deg/sec (+/- 250 dps range) & subtract calibration bias
    data_.gyroX = ((float)data_.rawGyroX / GYRO_SCALE_250DPS) - data_.gyroOffsetX;
    data_.gyroY = ((float)data_.rawGyroY / GYRO_SCALE_250DPS) - data_.gyroOffsetY;
    data_.gyroZ = ((float)data_.rawGyroZ / GYRO_SCALE_250DPS) - data_.gyroOffsetZ;

    // Calculate Total Resultant G-Force Magnitude
    data_.totalG = std::sqrt(data_.accelX * data_.accelX +
                             data_.accelY * data_.accelY +
                             data_.accelZ * data_.accelZ);

    // Compute Pitch & Roll Angles (in degrees) using Accelerometer Vector
    // Pitch: Rotation around Y-axis, Roll: Rotation around X-axis
    data_.pitch = std::atan2(data_.accelY, data_.accelZ) * (180.0f / M_PI);
    data_.roll  = std::atan2(-data_.accelX, std::sqrt(data_.accelY * data_.accelY + data_.accelZ * data_.accelZ)) * (180.0f / M_PI);
}

bool IMUSensor::isImpactDetected(float thresholdG) const {
    return data_.totalG >= thresholdG;
}

bool IMUSensor::isTumbleDetected(float thresholdDeg) const {
    return (std::abs(data_.pitch) >= thresholdDeg) || (std::abs(data_.roll) >= thresholdDeg);
}

bool IMUSensor::writeRegister(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    Wire.write(val);
    return (Wire.endTransmission() == 0);
}

bool IMUSensor::readRegisters(uint8_t reg, uint8_t* buffer, uint8_t length) {
    Wire.beginTransmission(address_);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) { // Send repeated start
        return false;
    }

    uint8_t readBytes = Wire.requestFrom(address_, length, (uint8_t)true);
    if (readBytes != length) {
        return false;
    }

    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = Wire.read();
    }
    return true;
}

