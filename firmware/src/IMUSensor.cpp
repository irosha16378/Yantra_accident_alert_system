#include "IMUSensor.h"
#include <math.h>

IMUSensor::IMUSensor(uint8_t i2cAddr)
    : _i2cAddr(i2cAddr),
      _initialized(false),
      _rawAccelX(0), _rawAccelY(0), _rawAccelZ(0),
      _rawGyroX(0), _rawGyroY(0), _rawGyroZ(0),
      _rawTemp(0),
      _accelOffsetX(0.0f), _accelOffsetY(0.0f), _accelOffsetZ(0.0f),
      _gyroOffsetX(0.0f), _gyroOffsetY(0.0f), _gyroOffsetZ(0.0f),
      _filterAlpha(0.2f),
      _data{0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 25.0f} {}

bool IMUSensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    Wire.write(value);
    return (Wire.endTransmission() == 0);
}

bool IMUSensor::readRegisters(uint8_t reg, uint8_t* buffer, size_t length) {
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;

    size_t readCount = Wire.requestFrom(_i2cAddr, (uint8_t)length, (uint8_t)true);
    if (readCount != length) return false;

    for (size_t i = 0; i < length; ++i) {
        buffer[i] = Wire.read();
    }
    return true;
}

bool IMUSensor::begin(int sdaPin, int sclPin) {
    Wire.begin(sdaPin, sclPin);
    Wire.setClock(400000); // 400kHz Fast I2C
    delay(100);

    // Verify WHO_AM_I register (should return 0x68 or 0x69)
    uint8_t whoAmI = 0;
    if (!readRegisters(MPU6050_REG_WHO_AM_I, &whoAmI, 1) || (whoAmI != 0x68 && whoAmI != 0x69)) {
        _initialized = false;
        Serial.print("[IMU ERROR] MPU6050 device not responding or invalid WHO_AM_I ID: 0x");
        Serial.println(whoAmI, HEX);
        return false;
    }

    // Reset PWR_MGMT_1 register to wake up device (CLKSEL = PLL with X axis gyro reference)
    if (!writeRegister(MPU6050_REG_PWR_MGMT_1, 0x01)) {
        _initialized = false;
        return false;
    }
    delay(10);

    // Configure DLPF (Digital Low Pass Filter) to ~44Hz bandwidth (CONFIG = 0x03)
    writeRegister(MPU6050_REG_CONFIG, 0x03);

    // Configure Gyro Full Scale Range ±250 deg/s (GYRO_CONFIG = 0x00)
    writeRegister(MPU6050_REG_GYRO_CONFIG, 0x00);

    // Configure Accel Full Scale Range ±8g (ACCEL_CONFIG = 0x10) -> Sensitivity 4096 LSB/g
    writeRegister(MPU6050_REG_ACCEL_CONFIG, 0x10);

    _initialized = true;
    Serial.print("[IMU] MPU6050 initialized successfully at address 0x");
    Serial.println(_i2cAddr, HEX);
    return true;
}

void IMUSensor::calibrate(uint16_t samples) {
    if (!_initialized) return;

    Serial.println("[IMU] Calibrating MPU6050 offsets... Keep device level and still.");
    int32_t sumAX = 0, sumAY = 0, sumAZ = 0;
    int32_t sumGX = 0, sumGY = 0, sumGZ = 0;

    for (uint16_t i = 0; i < samples; i++) {
        update();
        sumAX += _rawAccelX;
        sumAY += _rawAccelY;
        sumAZ += _rawAccelZ;
        sumGX += _rawGyroX;
        sumGY += _rawGyroY;
        sumGZ += _rawGyroZ;
        delay(5);
    }

    // Sensitivity scale for ±8g is 4096 LSB/g
    _accelOffsetX = (sumAX / (float)samples) / 4096.0f;
    _accelOffsetY = (sumAY / (float)samples) / 4096.0f;
    _accelOffsetZ = (sumAZ / (float)samples) / 4096.0f - 1.0f; // Subtract 1g gravity on Z

    // Sensitivity scale for ±250 deg/s is 131.0 LSB/(deg/s)
    _gyroOffsetX = (sumGX / (float)samples) / 131.0f;
    _gyroOffsetY = (sumGY / (float)samples) / 131.0f;
    _gyroOffsetZ = (sumGZ / (float)samples) / 131.0f;

    Serial.println("[IMU] Calibration complete!");
}

void IMUSensor::update() {
    if (!_initialized) return;

    // Read 14 bytes starting from ACCEL_XOUT_H (0x3B): Accel (6) + Temp (2) + Gyro (6)
    uint8_t buffer[14];
    if (!readRegisters(MPU6050_REG_ACCEL_XOUT_H, buffer, 14)) {
        return;
    }

    _rawAccelX = (int16_t)((buffer[0] << 8) | buffer[1]);
    _rawAccelY = (int16_t)((buffer[2] << 8) | buffer[3]);
    _rawAccelZ = (int16_t)((buffer[4] << 8) | buffer[5]);

    _rawTemp   = (int16_t)((buffer[6] << 8) | buffer[7]);

    _rawGyroX  = (int16_t)((buffer[8] << 8) | buffer[9]);
    _rawGyroY  = (int16_t)((buffer[10] << 8) | buffer[11]);
    _rawGyroZ  = (int16_t)((buffer[12] << 8) | buffer[13]);

    // Convert Accelerometer LSB to g (±8g scale -> 4096 LSB/g)
    _data.accelX = ((float)_rawAccelX / 4096.0f) - _accelOffsetX;
    _data.accelY = ((float)_rawAccelY / 4096.0f) - _accelOffsetY;
    _data.accelZ = ((float)_rawAccelZ / 4096.0f) - _accelOffsetZ;

    // Convert Gyroscope LSB to deg/s (±250 deg/s scale -> 131.0 LSB/deg/s)
    _data.gyroX = ((float)_rawGyroX / 131.0f) - _gyroOffsetX;
    _data.gyroY = ((float)_rawGyroY / 131.0f) - _gyroOffsetY;
    _data.gyroZ = ((float)_rawGyroZ / 131.0f) - _gyroOffsetZ;

    // Convert Temperature (Formula from datasheet: Temp = (rawTemp / 340.0) + 36.53)
    _data.temperature = ((float)_rawTemp / 340.0f) + 36.53f;

    // Total Acceleration Magnitude |G| = sqrt(Ax^2 + Ay^2 + Az^2)
    _data.totalG = sqrtf(_data.accelX * _data.accelX +
                         _data.accelY * _data.accelY +
                         _data.accelZ * _data.accelZ);

    // Exponential Moving Average filter for smooth G-Force tracking
    _data.filteredG = (_filterAlpha * _data.totalG) + ((1.0f - _filterAlpha) * _data.filteredG);

    // Calculate Pitch and Roll angles in degrees
    _data.pitch = atan2f(_data.accelY, sqrtf(_data.accelX * _data.accelX + _data.accelZ * _data.accelZ)) * (180.0f / M_PI);
    _data.roll  = atan2f(-_data.accelX, _data.accelZ) * (180.0f / M_PI);
}



