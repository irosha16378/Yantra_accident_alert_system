#ifndef HANDLEBAR_SENSOR_H
#define HANDLEBAR_SENSOR_H

#include <Arduino.h>
#include "Config.h"

/**
 * @brief Enum representing the current rider hand contact state on handlebars
 */
enum class GripState {
    NO_HANDS,   // Neither hand is touching the handlebar
    LEFT_ONLY,  // Only left hand is touching
    RIGHT_ONLY, // Only right hand is touching
    BOTH_HANDS  // Both hands are touching (safe riding state)
};

/**
 * @brief Structure containing complete handlebar sensing telemetry and state data
 */
struct HandlebarData {
    uint16_t rawLeft;           // Raw ADC/Capacitive reading for left grip
    uint16_t rawRight;          // Raw ADC/Capacitive reading for right grip
    float filteredLeft;         // Filtered EMA value for left grip
    float filteredRight;        // Filtered EMA value for right grip
    bool leftTouched;           // Left hand contact boolean flag
    bool rightTouched;          // Right hand contact boolean flag
    bool bothTouched;           // Both hands contact boolean flag
    bool handsOnBar;            // At least one hand on bar boolean flag
    GripState state;            // Current classified grip state enum
    uint32_t touchDurationMs;   // Continuous duration with hands on bar (ms)
    uint32_t handsOffDurationMs; // Continuous duration with hands off bar (ms)
};

/**
 * @brief Dual Handlebar Touch & Grip Sensor Driver for ESP32
 */
class HandlebarSensor {
private:
    uint8_t _pinLeft;
    uint8_t _pinRight;
    uint16_t _touchThreshold;
    uint16_t _hysteresis;
    float _filterAlpha;
    uint32_t _debounceMs;
    bool _initialized;
    bool _useTouchApi;          // True for touchRead, false for digitalRead

    // Internal state tracking
    HandlebarData _data;
    uint32_t _lastStateChangeTime;
    uint32_t _lastDebounceTimeLeft;
    uint32_t _lastDebounceTimeRight;
    bool _rawLeftState;
    bool _rawRightState;

    void readRawSensors();
    void applyFilter();
    void evaluateGripState();

public:
    HandlebarSensor();

    bool begin(uint8_t pinLeft = Config::PIN_HANDLEBAR_LEFT,
               uint8_t pinRight = Config::PIN_HANDLEBAR_RIGHT,
               uint16_t threshold = Config::TOUCH_THRESHOLD);

    void update();
    void calibrate(uint16_t samples = 100);

    // Threshold management
    void setTouchThreshold(uint16_t threshold) { _touchThreshold = threshold; }
    uint16_t getTouchThreshold() const { return _touchThreshold; }
    void setFilterAlpha(float alpha) { _filterAlpha = constrain(alpha, 0.05f, 1.0f); }

    // State queries
    const HandlebarData& getData() const { return _data; }
    GripState getGripState() const { return _data.state; }
    bool isLeftTouched() const { return _data.leftTouched; }
    bool isRightTouched() const { return _data.rightTouched; }
    bool isBothHandsOn() const { return _data.bothTouched; }
    bool isAnyHandOn() const { return _data.handsOnBar; }
    bool isHandsOffAlert(uint32_t timeoutMs = 3000) const;
    bool isInitialized() const { return _initialized; }

    uint16_t getRawLeft() const { return _data.rawLeft; }
    uint16_t getRawRight() const { return _data.rawRight; }
    float getFilteredLeft() const { return _data.filteredLeft; }
    float getFilteredRight() const { return _data.filteredRight; }

    // Diagnostics & Utilities
    void printStatus() const;
    void printDiagnostic() const;
    const char* getStateString() const;
};

#endif // HANDLEBAR_SENSOR_H
