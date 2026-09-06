#include "FusionEngine.h"

FusionEngine::FusionEngine(IMUSensor& imu,
                           HandlebarSensor& handlebar,
                           GPSManager& gps,
                           CellularManager& cellular)
    : _imu(imu),
      _handlebar(handlebar),
      _gps(gps),
      _cellular(cellular),
      _state(Config::SystemState::MONITORING),
      _countdownStartTime(0) {}

void FusionEngine::begin() {
    _state = Config::SystemState::MONITORING;
    Serial.println("[Fusion Engine] Sensor Fusion Decision Engine Ready.");
}

void FusionEngine::update() {
    switch (_state) {
        case Config::SystemState::MONITORING:
            handleMonitoring();
            break;

        case Config::SystemState::ACCIDENT_SUSPECTED:
            _countdownStartTime = millis();
            _state = Config::SystemState::COUNTDOWN;
            Serial.println("[ALERT] High Impact + Hands OFF detected! 10s Cancellation Countdown started...");
            break;

        case Config::SystemState::COUNTDOWN:
            handleCountdown();
            break;

        case Config::SystemState::ALERT_DISPATCHED:
            // Alert dispatched; maintain state until manual system reset
            break;

        case Config::SystemState::CANCELLED:
            Serial.println("[System] Rider cancelled alert. Resetting to monitoring state.");
            _state = Config::SystemState::MONITORING;
            break;
    }
}

void FusionEngine::handleMonitoring() {
    bool impactDetected = _imu.isImpactDetected();
    bool handsOff = _handlebar.areBothHandsOff();

    // Context-Aware Fusion logic:
    // High G-force alone (e.g. pothole/speedbump) is ignored if hands remain ON.
    // Accident condition: High G-force AND both hands off handlebars.
    if (impactDetected && handsOff) {
        _state = Config::SystemState::ACCIDENT_SUSPECTED;
    }
}

void FusionEngine::handleCountdown() {
    uint32_t elapsedTime = millis() - _countdownStartTime;

    if (elapsedTime >= Config::CANCELLATION_TIMEOUT_MS) {
        Serial.println("[CRITICAL] Countdown expired! Dispatching emergency cellular alert...");
        Config::LocationData loc = _gps.getLocation();
        _cellular.sendEmergencyAlert(loc);
        _state = Config::SystemState::ALERT_DISPATCHED;
    }
}

void FusionEngine::cancelAlert() {
    if (_state == Config::SystemState::COUNTDOWN) {
        _state = Config::SystemState::CANCELLED;
    }
}

Config::SystemState FusionEngine::getState() const {
    return _state;
}
