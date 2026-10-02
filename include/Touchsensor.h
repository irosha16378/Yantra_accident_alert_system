#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

#include <Arduino.h>

// ==========================================
// OOP CLASS: HandleTouchDetector
// Encapsulates Dual TTP223 Touch Sensors logic
// ==========================================
class HandleTouchDetector {
private:
    int leftPin;
    int rightPin;

public:
    // Constructor: Sets GPIO pins for Left and Right handle touch sensors
    HandleTouchDetector(int leftSensorPin, int rightSensorPin) {
        leftPin = leftSensorPin;
        rightPin = rightSensorPin;
    }

    // Initialize both touch sensor pins as INPUT
    void initTouchSensors() {
        pinMode(leftPin, INPUT);
        pinMode(rightPin, INPUT);
    }

    // Check if Left sensor is touched (HIGH when finger is on sensor)
    bool isLeftTouched() {
        return digitalRead(leftPin) == HIGH;
    }

    // Check if Right sensor is touched (HIGH when finger is on sensor)
    bool isRightTouched() {
        return digitalRead(rightPin) == HIGH;
    }

    // Check if BOTH fingers/hands are removed from the handle
    // Returns TRUE only when BOTH sensors are NOT touched (LOW)
    bool areBothHandsRemoved() {
        return (!isLeftTouched() && !isRightTouched());
    }
};

#endif