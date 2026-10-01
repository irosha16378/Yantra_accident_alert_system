#include <Arduino.h>
#include "Gyroscopesensor.h" // Use your actual Gyroscope header file name here
#include "GpsTracker.h"     // Include our new GPS class

// Create Sensor Objects
MotionDetector bikeSensor(2.5);
GpsTracker bikeGps(16, 17, 9600); // RX2 = GPIO 16, TX2 = GPIO 17, Baud = 9600

void setup() {
    Serial.begin(115200);
    Wire.begin();

    Serial.println("Initializing System...");

    // 1. Initialize Gyroscope
    if (!bikeSensor.initSensor()) {
        Serial.println("Error: MPU6050 Sensor not found!");
        while (1) { delay(10); }
    }

    // 2. Initialize GPS
    bikeGps.initGps();
    Serial.println("System Ready! Monitoring for vibrations & GPS data...");
}

void loop() {
    // Always keep reading GPS data in the background
    bikeGps.updateGps();

    // Check if vibration/impact is detected
    if (bikeSensor.isVibrationDetected()) {
        Serial.println("\n⚠️ ALERT: High Vibration / Impact Detected!");
        
        // Check if GPS wiring is working
        if (!bikeGps.isWiringOk()) {
            Serial.println("❌ GPS Error: No data received! Check RX/TX wiring.");
        } else {
            // Print location or Google Maps link
            Serial.print("📍 Location: ");
            Serial.println(bikeGps.getGoogleMapsLink());
        }
        Serial.println("----------------------------------------\n");
        
        delay(1000);
    }

    delay(100);
}