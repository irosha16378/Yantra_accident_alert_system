#include "HandlebarSensor.h"

HandlebarSensor::HandlebarSensor()
    : _pinLeft(Config::PIN_HANDLEBAR_LEFT),
      _pinRight(Config::PIN_HANDLEBAR_RIGHT),
      _touchThreshold(Config::TOUCH_THRESHOLD),
      _hysteresis(5),
      _filterAlpha(0.25f),
      _debounceMs(50),
      _initialized(false),
      _useTouchApi(true),
      _lastStateChangeTime(0),
      _lastDebounceTimeLeft(0),
      _lastDebounceTimeRight(0),
      _rawLeftState(false),
      _rawRightState(false) {
    _data = {};
    _data.state = GripState::NO_HANDS;
}

bool HandlebarSensor::begin(uint8_t pinLeft, uint8_t pinRight, uint16_t threshold) {
    _pinLeft = pinLeft;
    _pinRight = pinRight;
    _touchThreshold = threshold;
    _initialized = true;
    return true;
}

void HandlebarSensor::update() {
}

void HandlebarSensor::calibrate(uint16_t samples) {
    (void)samples;
}

void HandlebarSensor::readRawSensors() {
}

void HandlebarSensor::applyFilter() {
}

void HandlebarSensor::evaluateGripState() {
}

bool HandlebarSensor::isHandsOffAlert(uint32_t timeoutMs) const {
    (void)timeoutMs;
    return false;
}

void HandlebarSensor::printStatus() const {
}

void HandlebarSensor::printDiagnostic() const {
}

const char* HandlebarSensor::getStateString() const {
    switch (_data.state) {
        case GripState::NO_HANDS:   return "NO_HANDS";
        case GripState::LEFT_ONLY:  return "LEFT_ONLY";
        case GripState::RIGHT_ONLY: return "RIGHT_ONLY";
        case GripState::BOTH_HANDS: return "BOTH_HANDS";
        default:                    return "UNKNOWN";
    }
}
