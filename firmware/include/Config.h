#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
    constexpr uint32_t MONITOR_BAUD = 115200;

    // I2C Motion Sensor (MPU6050) Pins
    constexpr uint8_t PIN_SDA = 21;
    constexpr uint8_t PIN_SCL = 22;
}


#endif // CONFIG_H

