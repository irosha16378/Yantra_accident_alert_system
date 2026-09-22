#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>

class IMUSensor {
public:
    IMUSensor();
    bool begin(uint8_t sdaPin, uint8_t sclPin);
    void update();
};

#endif // IMU_SENSOR_H
