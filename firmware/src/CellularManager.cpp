#include "CellularManager.h"

CellularManager::CellularManager(HardwareSerial& serial)
    : _serial(serial),
      _rxPin(Config::PIN_MODEM_RX),
      _txPin(Config::PIN_MODEM_TX),
      _pwrPin(Config::PIN_MODEM_PWR),
      _baudRate(Config::MODEM_BAUD),
      _initialized(false),
      _registered(false),
      _csq(99),
      _operatorName("Unknown") {}

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

    // Check SIM ready status
    if (checkSIMReady()) {
        Serial.println("[CellularManager] SIM Card Ready.");
    } else {
        Serial.println("[CellularManager] [WARNING] SIM Card not ready or PIN locked.");
    }

    // Check network registration
    _registered = checkNetworkRegistration();
    _csq = querySignalQuality();
    _operatorName = queryNetworkOperator();

    Serial.print("[CellularManager] Network Registered: ");
    Serial.println(_registered ? "YES" : "NO");
    Serial.print("[CellularManager] Signal Quality (CSQ): ");
    Serial.println(_csq);
    Serial.print("[CellularManager] Network Operator: ");
    Serial.println(_operatorName);

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

bool CellularManager::checkSIMReady() {
    String response;
    if (sendATCommandWithResponse("AT+CPIN?", response, 2000)) {
        return (response.indexOf("READY") != -1);
    }
    return false;
}

bool CellularManager::checkNetworkRegistration() {
    String response;
    // Check CREG (GSM) or CEREG (LTE/4G)
    if (sendATCommandWithResponse("AT+CREG?", response, 2000)) {
        if (response.indexOf(",1") != -1 || response.indexOf(",5") != -1) {
            return true;
        }
    }
    if (sendATCommandWithResponse("AT+CEREG?", response, 2000)) {
        if (response.indexOf(",1") != -1 || response.indexOf(",5") != -1) {
            return true;
        }
    }
    return false;
}

int CellularManager::querySignalQuality() {
    String response;
    if (sendATCommandWithResponse("AT+CSQ", response, 2000)) {
        int csqIndex = response.indexOf("+CSQ: ");
        if (csqIndex != -1) {
            int commaIndex = response.indexOf(',', csqIndex);
            if (commaIndex != -1) {
                String valStr = response.substring(csqIndex + 6, commaIndex);
                valStr.trim();
                _csq = valStr.toInt();
                return _csq;
            }
        }
    }
    _csq = 99;
    return _csq;
}

String CellularManager::queryNetworkOperator() {
    String response;
    if (sendATCommandWithResponse("AT+COPS?", response, 2000)) {
        int quoteStart = response.indexOf('"');
        int quoteEnd = response.indexOf('"', quoteStart + 1);
        if (quoteStart != -1 && quoteEnd != -1) {
            _operatorName = response.substring(quoteStart + 1, quoteEnd);
            return _operatorName;
        }
    }
    _operatorName = "Unknown";
    return _operatorName;
}
