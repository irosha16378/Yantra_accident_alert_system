#include "CellularManager.h"

CellularManager::CellularManager(HardwareSerial& serial)
    : _serial(serial),
      _rxPin(Config::PIN_MODEM_RX),
      _txPin(Config::PIN_MODEM_TX),
      _pwrPin(Config::PIN_MODEM_PWR),
      _baudRate(Config::MODEM_BAUD),
      _initialized(false) {}

bool CellularManager::begin(int rxPin, int txPin, int pwrPin, uint32_t baudRate) {
    _rxPin = rxPin;
    _txPin = txPin;
    _pwrPin = pwrPin;
    _baudRate = baudRate;

    // Configure modem power key pin
    if (_pwrPin >= 0) {
        pinMode(_pwrPin, OUTPUT);
        digitalWrite(_pwrPin, LOW);
    }

    // Initialize UART port for modem
    _serial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
    delay(500);

    Serial.println("[CellularManager] Hardware UART and GPIO pins initialized.");

    _initialized = true;
    return true;
}

void CellularManager::powerOn() {
    if (_pwrPin < 0) return;

    Serial.println("[CellularManager] Powering on modem hardware...");
    digitalWrite(_pwrPin, HIGH);
    delay(1500); // 1.5 second power key pulse
    digitalWrite(_pwrPin, LOW);
    delay(3000); // Wait for modem boot sequence
}

void CellularManager::reset() {
    if (_pwrPin < 0) return;

    Serial.println("[CellularManager] Resetting modem hardware...");
    digitalWrite(_pwrPin, HIGH);
    delay(3000); // Extended pulse to reset
    digitalWrite(_pwrPin, LOW);
    delay(5000);
}
