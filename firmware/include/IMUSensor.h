#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>

class IMUSensor {
public:
    IMUSensor();
    bool begin();
    void update();
};

#endif // IMU_SENSOR_H

