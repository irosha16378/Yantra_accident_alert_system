#include <Arduino.h>
#include "Gyroscopesensor.h" // Gyroscope & Accelerometer sensor class
#include "GpsTracker.h"      // NEO-6M GPS module class
#include "TouchSensor.h"     // Dual TTP223 Touch sensor class

// ==========================================
// CREATE SENSOR OBJECTS
// ==========================================
MotionDetector bikeSensor(2.5);       // Vibration threshold set to 2.5
GpsTracker bikeGps(16, 17, 9600);     // RX2 = GPIO 16, TX2 = GPIO 17, Baud = 9600
HandleTouchDetector bikeHandle(4, 5); // Left Touch = GPIO 4, Right Touch = GPIO 5

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Initialize I2C communication for MPU6050
    Wire.begin();

    Serial.println("Initializing System...");

    // 1. Initialize Gyroscope Sensor
    if (!bikeSensor.initSensor()) {
        Serial.println("Error: MPU6050 Sensor not found! Check wiring.");
        while (1) { 
            delay(10); // Halt program if gyroscope fails
        }
    }

    // 2. Initialize GPS Module
    bikeGps.initGps();

    // 3. Initialize Handlebar Touch Sensors
    bikeHandle.initTouchSensors();

    Serial.println("System Ready! Monitoring for vibrations, touch & GPS data...");
}

void loop() {
    // Continuously read incoming satellite data from the GPS module
    bikeGps.updateGps();

    // Check rider's hand status on the handlebar (for testing/debugging)
    if (bikeHandle.areBothHandsRemoved()) {
        Serial.println("🖐️ WARNING: Both hands removed from handle!");
    } else {
        Serial.println("✅ Rider is holding the handle.");
    }

    // Main Accident Detection Logic:
    // Trigger alert ONLY if high vibration is detected AND both hands are off the handle
    if (bikeSensor.isVibrationDetected() && bikeHandle.areBothHandsRemoved()) {
        Serial.println("\n🚨 CRITICAL ALERT: Impact Detected & Rider Separated from Bike!");
        
        // Check if GPS wiring is functional and print location link
        if (!bikeGps.isWiringOk()) {
            Serial.println("❌ GPS Error: No data received! Check RX/TX wiring.");
        } else {
            Serial.print("📍 Location: ");
            Serial.println(bikeGps.getGoogleMapsLink());
        }
        Serial.println("----------------------------------------\n");
        
        // Wait 1 second after an alert to prevent spamming
        delay(1000);
    }

    // Main loop delay (200 milliseconds)
    delay(200);
}