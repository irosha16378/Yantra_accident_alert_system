#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "Gyroscopesensor.h"       // Gyroscope & Accelerometer sensor class
#include "GpsTracker.h"            // NEO-6M GPS module class
#include "TouchSensor.h"           // Dual TTP223 Touch sensor class

// ==========================================
// WIFI & FIREBASE CONFIGURATION
// ==========================================
#define WIFI_SSID "Oyaage_Hotspot_Name"       // ඔයාගේ ෆෝන් හෝ රවුටර් Wi-Fi නම දාන්න
#define WIFI_PASSWORD "Oyaage_Hotspot_Password" // Wi-Fi පාස්වර්ඩ් එක දාන්න

#define FIREBASE_HOST "https://yantra-accident-alert-system-default-rtdb.firebaseio.com/" // ඔයාගේ Database URL එක
#define FIREBASE_AUTH "Oyaage_Database_Secret_Key"                                     // අර කලින් ගත්තු Secret Key එක

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ==========================================
// CREATE SENSOR OBJECTS
// ==========================================
MotionDetector bikeSensor(2.5);       // Vibration threshold set to 2.5
GpsTracker bikeGps(16, 17, 9600);    // RX2 = GPIO 16, TX2 = GPIO 17, Baud = 9600
HandleTouchDetector bikeHandle(4, 5); // Left Touch = GPIO 4, Right Touch = GPIO 5

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Initialize I2C communication for MPU6050
    Wire.begin();

    Serial.println("Initializing System...");

    // Start Wi-Fi connection
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected Successfully!");

    // Initialize Firebase Configuration
    config.host = FIREBASE_HOST;
    config.signer.test_key = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Firebase Cloud Connected!");

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

    // System පටන් ගන්නකොට ඩේටාබේස් එකට SAFE කියලා දානවා
    Firebase.setString(fbdo, "/accident_alert/status", "SAFE");

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
        // PREPARE AND SEND DATA TO FIREBASE CLOUD
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

        // Send CRITICAL ALERT status and GPS coordinates directly to Firebase Cloud
        Firebase.setString(fbdo, "/accident_alert/status", "CRITICAL_ALERT");
        Firebase.setFloat(fbdo, "/accident_alert/latitude", currentLat);
        Firebase.setFloat(fbdo, "/accident_alert/longitude", currentLng);
        
        Serial.println("Data successfully uploaded to Firebase Cloud!");
        Serial.println("----------------------------------------\n");
        
        // Wait 5 seconds after an alert to prevent spamming
        delay(5000);
    }

    // Main loop delay (200 milliseconds)
    delay(200);
}