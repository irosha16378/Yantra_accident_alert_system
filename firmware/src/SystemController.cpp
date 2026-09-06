#include "SystemController.h"

SystemController::SystemController()
    : _imu(),
      _handlebar(),
      _gps(),
      _cellular(),
      _fusionEngine(_imu, _handlebar, _gps, _cellular),
      _lastHeartbeat(0) {}

void SystemController::begin() {
    Serial.begin(Config::MONITOR_BAUD);
    delay(1000);

    Serial.println("=================================================");
    Serial.println("  YANTRA Motorcycle Accident Alert System (ESP32)");
    Serial.println("=================================================");

    _imu.begin();
    _handlebar.begin();
    _gps.begin();
    _cellular.begin();
    _fusionEngine.begin();

    pinMode(Config::PIN_CANCEL_BTN, INPUT_PULLUP);
    Serial.println("[SystemController] All subsystems initialized successfully.\n");
}

void SystemController::update() {
    // Poll hardware inputs/sensors
    _imu.update();
    _gps.update();

    // Monitor cancel button for rider override
    if (digitalRead(Config::PIN_CANCEL_BTN) == LOW) {
        _fusionEngine.cancelAlert();
    }

    // Execute state machine logic
    _fusionEngine.update();

    // Output status heartbeat every 5 seconds
    if (millis() - _lastHeartbeat >= 5000) {
        _lastHeartbeat = millis();
        Serial.println("[System Heartbeat] System active & monitoring...");
    }
}
