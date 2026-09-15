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

    Serial.println("[CellularManager] Initializing UART modem communication...");

    // Pulse power key if modem is not responding
    if (!checkModemResponse(1000)) {
        Serial.println("[CellularManager] Modem unresponsive. Toggling power key...");
        powerOn();
    }

    // Verify communication with AT
    if (!checkModemResponse(3000)) {
        Serial.println("[CellularManager] [ERROR] Modem did not respond to AT commands.");
        _initialized = false;
        return false;
    }

    Serial.println("[CellularManager] Modem responding!");

    // Disable echo for cleaner parsing
    sendATCommand("ATE0");

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

void CellularManager::clearBuffer() {
    while (_serial.available()) {
        _serial.read();
    }
}

bool CellularManager::sendATCommand(const String& cmd, const char* expectedReply, uint32_t timeoutMs) {
    String response;
    return sendATCommandWithResponse(cmd, response, timeoutMs) && (response.indexOf(expectedReply) != -1);
}

bool CellularManager::sendATCommandWithResponse(const String& cmd, String& response, uint32_t timeoutMs) {
    clearBuffer();
    _serial.println(cmd);

    response = "";
    uint32_t startMs = millis();

    while (millis() - startMs < timeoutMs) {
        while (_serial.available()) {
            char c = (char)_serial.read();
            response += c;
        }
        if (response.indexOf("OK") != -1 || response.indexOf("ERROR") != -1) {
            break;
        }
        delay(10);
    }
    return response.length() > 0;
}

bool CellularManager::checkModemResponse(uint32_t timeoutMs) {
    return sendATCommand("AT", "OK", timeoutMs);
}
