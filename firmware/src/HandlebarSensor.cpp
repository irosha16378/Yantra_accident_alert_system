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

    Serial.println("[HANDLEBAR] Initializing Dual Handlebar Touch Sensors...");
    Serial.printf("[HANDLEBAR] Configuration: Left GPIO%d | Right GPIO%d | Touch Threshold: %d\n",
                  _pinLeft, _pinRight, _touchThreshold);

    // Set pin modes for digital fallback
    pinMode(_pinLeft, INPUT_PULLUP);
    pinMode(_pinRight, INPUT_PULLUP);

    // Initial sensor warm-up read
    #if defined(ESP32)
        _useTouchApi = true;
        _data.rawLeft = touchRead(_pinLeft);
        _data.rawRight = touchRead(_pinRight);
    #else
        _useTouchApi = false;
        _data.rawLeft = digitalRead(_pinLeft) == LOW ? 0 : 100;
        _data.rawRight = digitalRead(_pinRight) == LOW ? 0 : 100;
    #endif

    _data.filteredLeft = (float)_data.rawLeft;
    _data.filteredRight = (float)_data.rawRight;
    _data.state = GripState::NO_HANDS;
    _data.touchDurationMs = 0;
    _data.handsOffDurationMs = 0;
    _lastStateChangeTime = millis();

    _initialized = true;
    Serial.printf("[HANDLEBAR] Initial readings -> Left: %d | Right: %d\n", _data.rawLeft, _data.rawRight);
    Serial.println("[SUCCESS] Handlebar sensor hardware initialized.");

    return true;
}

void HandlebarSensor::update() {
}

void HandlebarSensor::calibrate(uint16_t samples) {
    (void)samples;
}

void HandlebarSensor::readRawSensors() {
    if (!_initialized) return;

    #if defined(ESP32)
        if (_useTouchApi) {
            _data.rawLeft = touchRead(_pinLeft);
            _data.rawRight = touchRead(_pinRight);
        } else {
            _data.rawLeft = (digitalRead(_pinLeft) == LOW) ? 10 : 80;
            _data.rawRight = (digitalRead(_pinRight) == LOW) ? 10 : 80;
        }
    #else
        _data.rawLeft = (digitalRead(_pinLeft) == LOW) ? 10 : 80;
        _data.rawRight = (digitalRead(_pinRight) == LOW) ? 10 : 80;
    #endif
}

void HandlebarSensor::applyFilter() {
    if (!_initialized) return;

    // Exponential Moving Average (EMA) smoothing
    _data.filteredLeft  = (_filterAlpha * _data.rawLeft)  + ((1.0f - _filterAlpha) * _data.filteredLeft);
    _data.filteredRight = (_filterAlpha * _data.rawRight) + ((1.0f - _filterAlpha) * _data.filteredRight);

    // Dynamic threshold determination with capacitive touch polarity
    bool currentRawLeft  = (_data.filteredLeft <= (float)_touchThreshold);
    bool currentRawRight = (_data.filteredRight <= (float)_touchThreshold);

    uint32_t now = millis();

    // Debounce left touch signal
    if (currentRawLeft != _rawLeftState) {
        if (now - _lastDebounceTimeLeft >= _debounceMs) {
            _rawLeftState = currentRawLeft;
            _lastDebounceTimeLeft = now;
        }
    } else {
        _lastDebounceTimeLeft = now;
    }

    // Debounce right touch signal
    if (currentRawRight != _rawRightState) {
        if (now - _lastDebounceTimeRight >= _debounceMs) {
            _rawRightState = currentRawRight;
            _lastDebounceTimeRight = now;
        }
    } else {
        _lastDebounceTimeRight = now;
    }

    _data.leftTouched  = _rawLeftState;
    _data.rightTouched = _rawRightState;
}

void HandlebarSensor::evaluateGripState() {
    if (!_initialized) return;

    _data.bothTouched = (_data.leftTouched && _data.rightTouched);
    _data.handsOnBar  = (_data.leftTouched || _data.rightTouched);

    GripState newState;
    if (_data.leftTouched && _data.rightTouched) {
        newState = GripState::BOTH_HANDS;
    } else if (_data.leftTouched) {
        newState = GripState::LEFT_ONLY;
    } else if (_data.rightTouched) {
        newState = GripState::RIGHT_ONLY;
    } else {
        newState = GripState::NO_HANDS;
    }

    uint32_t now = millis();

    if (newState != _data.state) {
        Serial.printf("[HANDLEBAR] Grip State Transition: %s -> %s (Left: %s, Right: %s)\n",
                      getStateString(),
                      (newState == GripState::BOTH_HANDS ? "BOTH_HANDS" :
                       newState == GripState::LEFT_ONLY  ? "LEFT_ONLY" :
                       newState == GripState::RIGHT_ONLY ? "RIGHT_ONLY" : "NO_HANDS"),
                      _data.leftTouched ? "TOUCHED" : "RELEASED",
                      _data.rightTouched ? "TOUCHED" : "RELEASED");

        _data.state = newState;
        _lastStateChangeTime = now;
        _data.touchDurationMs = 0;
        _data.handsOffDurationMs = 0;
    } else {
        if (_data.handsOnBar) {
            _data.touchDurationMs = now - _lastStateChangeTime;
            _data.handsOffDurationMs = 0;
        } else {
            _data.handsOffDurationMs = now - _lastStateChangeTime;
            _data.touchDurationMs = 0;
        }
    }
}

bool HandlebarSensor::isHandsOffAlert(uint32_t timeoutMs) const {
    return (!_data.handsOnBar && (_data.handsOffDurationMs >= timeoutMs));
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
