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
    _initialized = true;
    return true;
}
