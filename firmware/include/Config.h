#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/**
 * @file Config.h
 * @brief Centralized Hardware Pinouts & System Threshold Parameters
 * 
 * Defines all GPIO assignments, serial baud rates, I2C addresses, and 
 * threshold values for the Yantra Accident Alert System.
 */
namespace Config {
    // ==========================================
    // Serial Monitor & Communication Configuration
    // ==========================================
    constexpr uint32_t MONITOR_BAUD = 115200;

    // ==========================================
    // Motion Sensor (MPU6050) I2C Configuration
    // ==========================================
    constexpr uint8_t PIN_SDA = 21;
    constexpr uint8_t PIN_SCL = 22;
    constexpr uint32_t I2C_CLOCK_HZ = 400000;
    constexpr uint8_t MPU6050_I2C_ADDR = 0x68;

    // ==========================================
    // Handlebar Hand Detection Sensor GPIO Pins
    // ==========================================
    constexpr uint8_t PIN_HANDLEBAR_LEFT  = 4;   // Touch / Digital Pin (Left Grip)
    constexpr uint8_t PIN_HANDLEBAR_RIGHT = 15;  // Touch / Digital Pin (Right Grip)
    constexpr uint16_t TOUCH_THRESHOLD    = 40;  // Capacitive touch threshold value

    // ==========================================
    // GPS Module (NMEA UART) Pins & Config
    // ==========================================
    constexpr uint8_t PIN_GPS_RX = 16;  // ESP32 RX2 connected to GPS TX
    constexpr uint8_t PIN_GPS_TX = 17;  // ESP32 TX2 connected to GPS RX
    constexpr uint32_t GPS_BAUD  = 9600;

    // ==========================================
    // 4G Cellular Communication Module (UART) Pins
    // ==========================================
    constexpr uint8_t PIN_MODEM_RX   = 18;  // ESP32 RX1 connected to Modem TX
    constexpr uint8_t PIN_MODEM_TX   = 19;  // ESP32 TX1 connected to Modem RX
    constexpr uint8_t PIN_MODEM_PWR  = 5;   // Modem Power Key / Reset Pin
    constexpr uint32_t MODEM_BAUD    = 115200;

    // ==========================================
    // Alert / Status Feedback Hardware Pins
    // ==========================================
    constexpr uint8_t PIN_BUZZER     = 25;  // Warning Piezo Buzzer Pin
    constexpr uint8_t PIN_STATUS_LED = 2;   // Status Indicator LED (Onboard / External)

    // ==========================================
    // Accident Detection Algorithm Parameters
    // ==========================================
    constexpr float ACCIDENT_IMPACT_THRESHOLD_G  = 3.0f;   // Total G-force impact trigger
    constexpr float TUMBLE_ANGLE_THRESHOLD_DEG   = 60.0f;  // Tilt angle trigger (Pitch/Roll)
    constexpr uint32_t CANCELLATION_TIMEOUT_MS   = 10000;  // Rider cancel countdown (10 sec)
    constexpr uint32_t SAMPLING_INTERVAL_MS      = 10;     // 100Hz Sensing Loop interval
}

#endif // CONFIG_H


