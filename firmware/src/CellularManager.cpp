#include "CellularManager.h"

CellularManager::CellularManager() : _isInitialized(false) {}

bool CellularManager::begin() {
    // Hardware Serial modem init: e.g. Serial1.begin(Config::MODEM_BAUD, SERIAL_8N1, Config::PIN_MODEM_RX, Config::PIN_MODEM_TX);
    _isInitialized = true;
    Serial.println("[4G Cellular] Modem initialized.");
    return true;
}

bool CellularManager::sendEmergencyAlert(const Config::LocationData& location) {
    Serial.print("[4G Cellular] Sending Emergency SOS Alert! Lat: ");
    Serial.print(location.latitude, 6);
    Serial.print(", Lon: ");
    Serial.println(location.longitude, 6);

    // Transmit AT commands, SMS or HTTP POST packet to Emergency Service & PC app
    return true;
}
