#ifndef HANDLEBAR_SENSOR_H
#define HANDLEBAR_SENSOR_H

#include <Arduino.h>
#include "Config.h"

class HandlebarSensor {
private:
    uint8_t _pinLeft;
    uint8_t _pinRight;

public:
    HandlebarSensor(uint8_t pinLeft = Config::PIN_HANDLEBAR_LEFT,
                    uint8_t pinRight = Config::PIN_HANDLEBAR_RIGHT);

    void begin();
    bool isLeftHandOn() const;
    bool isRightHandOn() const;
    bool areBothHandsOff() const;
};

#endif // HANDLEBAR_SENSOR_H
