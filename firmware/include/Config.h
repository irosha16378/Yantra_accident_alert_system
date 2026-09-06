#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
    // Serial Baud Rates
    constexpr uint32_t MONITOR_BAUD = 115200;
    constexpr uint32_t GPS_BAUD     = 9600;
    constexpr uint32_t MODEM_BAUD   = 115200;

    // Hardware Pin Configurations (ESP32 / ESP32-S3)
    constexpr uint8_t PIN_SDA           = 21;
    constexpr uint8_t PIN_SCL           = 22;
    constexpr uint8_t PIN_HANDLEBAR_LEFT  = 4;
    constexpr uint8_t PIN_HANDLEBAR_RIGHT = 5;
    constexpr uint8_t PIN_GPS_RX        = 16;
    constexpr uint8_t PIN_GPS_TX        = 17;
    constexpr uint8_t PIN_MODEM_RX      = 26;
    constexpr uint8_t PIN_MODEM_TX      = 27;
    constexpr uint8_t PIN_BUZZER        = 18;
    constexpr uint8_t PIN_CANCEL_BTN    = 19;

    // Sensor Fusion Thresholds
    constexpr float ACCEL_IMPACT_THRESHOLD_G = 3.5f;   // Crash G-force threshold
    constexpr uint32_t CANCELLATION_TIMEOUT_MS = 10000; // 10-second countdown

    // Location Data Structure
    struct LocationData {
        float latitude = 0.0f;
        float longitude = 0.0f;
        bool valid = false;
    };

    // System State Machine States
    enum class SystemState {
        MONITORING,
        ACCIDENT_SUSPECTED,
        COUNTDOWN,
        ALERT_DISPATCHED,
        CANCELLED
    };
}

#endif // CONFIG_H
