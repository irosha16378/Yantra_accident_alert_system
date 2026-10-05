#include <Arduino.h>
#include "Gyroscopesensor.h"       // Gyroscope & Accelerometer sensor class
#include "GpsTracker.h"            // NEO-6M GPS module class
#include "TouchSensor.h"           // Dual TTP223 Touch sensor class
#include "DashboardConnector.h"    // Web dashboard connector class

// ==========================================
// CREATE SENSOR OBJECTS
// ==========================================
MotionDetector bikeSensor(2.5);       // Vibration threshold set to 2.5
GpsTracker bikeGps(16, 17, 9600);     // RX2 = GPIO 16, TX2 = GPIO 17, Baud = 9600
HandleTouchDetector bikeHandle(4, 5); // Left Touch = GPIO 4, Right Touch = GPIO 5

// ==========================================
// WIFI & DASHBOARD CONFIGURATION
// Replace with your actual WiFi details and Laptop IP
// ==========================================
DashboardConnector webDash("Your_WiFi_Name", "Your_Password", "http://192.168.8.156:5000/api/alert");

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Initialize I2C communication for MPU6050
    Wire.begin();

    Serial.println("Initializing System...");

    // Start the Wi-Fi connection
    webDash.initWiFi();

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

    // Main Accident Detection Logic:
    // Trigger alert ONLY if high vibration is detected AND both hands are off the handle
    if (bikeSensor.isVibrationDetected() && bikeHandle.areBothHandsRemoved()) {
        Serial.println("\n🚨 CRITICAL ALERT: Impact Detected & Rider Separated from Bike!");
        
        // ==========================================
        // PREPARE AND SEND DATA TO DASHBOARD
        // ==========================================
        double currentLat = 6.7146;  // Default latitude (if GPS signal is lost)
        double currentLng = 80.7872; // Default longitude (if GPS signal is lost)
        
        // Check if GPS wiring is functional and update coordinates
        if (!bikeGps.isWiringOk()) {
            Serial.println("❌ GPS Error: No data received! Check RX/TX wiring.");
        } else if (bikeGps.isLocationValid()) {
            currentLat = bikeGps.getLatitude();
            currentLng = bikeGps.getLongitude();
            Serial.print("📍 Location Locked: ");
            Serial.println(bikeGps.getGoogleMapsLink());
        }

        // Send the real-time alert data to the Python Server over Wi-Fi
        webDash.sendAlert(currentLat, currentLng);
        
        Serial.println("----------------------------------------\n");
        
        // Wait 5 seconds after an alert to prevent server spamming
        delay(5000);
    }

    // Main loop delay (200 milliseconds)
    delay(200);
}