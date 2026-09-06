#include "GPSManager.h"

GPSManager::GPSManager() : _isInitialized(false) {}

bool GPSManager::begin() {
    // Hardware UART Initialization: e.g. Serial2.begin(Config::GPS_BAUD, SERIAL_8N1, Config::PIN_GPS_RX, Config::PIN_GPS_TX);
    _isInitialized = true;
    Serial.println("[GPS] Module initialized.");
    return true;
}

void GPSManager::update() {
    if (!_isInitialized) return;
    // Process incoming NMEA stream (TinyGPS++ parser logic)
}

Config::LocationData GPSManager::getLocation() const {
    return _currentLocation;
}

bool GPSManager::hasValidFix() const {
    return _currentLocation.valid;
}
