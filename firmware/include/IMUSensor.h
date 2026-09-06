#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include <Arduino.h>
#include "Config.h"

class IMUSensor {
private:
    float _lastGForce;
    float _impactThreshold;
    bool _isInitialized;

public:
    explicit IMUSensor(float impactThreshold = Config::ACCEL_IMPACT_THRESHOLD_G);

    bool begin();
    void update();
    bool isImpactDetected() const;
    float getCurrentGForce() const;
};

#endif // IMU_SENSOR_H
