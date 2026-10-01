#ifndef GPS_TRACKER_H
#define GPS_TRACKER_H

#include <Arduino.h>
#include <TinyGPS++.h>

// ==========================================
// OOP CLASS: GpsTracker
// Encapsulates NEO-6M GPS module logic
// ==========================================
class GpsTracker {
private:
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    int rxPin;
    int txPin;
    long baudRate;

public:
    // Constructor: Sets RX, TX pins and Baud Rate (Default for NEO-6M is 9600)
    GpsTracker(int rx, int tx, long baud = 9600) {
        rxPin = rx;
        txPin = tx;
        baudRate = baud;
        gpsSerial = &Serial2; // Using Hardware Serial 2 of ESP32
    }

    // Initialize GPS Serial communication
    void initGps() {
        gpsSerial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
    }

    // Continuously read data from GPS module (Must be called inside loop)
    void updateGps() {
        while (gpsSerial->available() > 0) {
            gps.encode(gpsSerial->read());
        }
    }

    // Check if the wiring is correct and ESP32 is receiving data from GPS
    bool isWiringOk() {
        return (gps.charsProcessed() > 10);
    }

    // Check if GPS has locked onto satellites and has a valid location
    bool isLocationValid() {
        return gps.location.isValid();
    }

    // Get Latitude
    double getLatitude() {
        return gps.location.lat();
    }

    // Get Longitude
    double getLongitude() {
        return gps.location.lng();
    }

    // Generate a direct Google Maps link for the current location
    String getGoogleMapsLink() {
        if (isLocationValid()) {
            return "https://www.google.com/maps?q=" + String(getLatitude(), 6) + "," + String(getLongitude(), 6);
        } else {
           return "https://www.google.com/maps?q=6.7146,80.7872 (Searching Satellites...)";
        }
    }
};

#endif