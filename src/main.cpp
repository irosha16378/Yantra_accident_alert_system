#include <Arduino.h>
#include "Gyroscopesensor.h" // Include our custom sensor class

// Create Gyroscope sensor object
MotionDetector bikeSensor(2.5);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    Serial.println("Initializing System...");

    // Initialize Gyroscope
    if (!bikeSensor.initSensor()) {
        Serial.println("Error: MPU6050 Sensor not found!");
        while (1) {
            delay(10);
        }
    }

    Serial.println("System Ready!");
}

void loop() {
    // Check vibration from MotionDetector
    if (bikeSensor.isVibrationDetected()) {
        Serial.println("⚠️ ALERT: High Vibration / Impact Detected!");
        delay(1000);
    }

    delay(200);
}