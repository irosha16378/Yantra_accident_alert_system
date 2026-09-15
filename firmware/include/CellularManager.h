#ifndef CELLULAR_MANAGER_H
#define CELLULAR_MANAGER_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "Config.h"

/**
 * @file CellularManager.h
 * @brief 4G/LTE Cellular Communications Driver for Yantra Accident Alert System
 */
class CellularManager {
private:
    HardwareSerial& _serial;
    int _rxPin;
    int _txPin;
    int _pwrPin;
    uint32_t _baudRate;
    bool _initialized;

public:
    explicit CellularManager(HardwareSerial& serial = Serial1);

    /**
     * @brief Initialize cellular hardware UART and modem pins
     */
    bool begin(int rxPin = Config::PIN_MODEM_RX, 
               int txPin = Config::PIN_MODEM_TX, 
               int pwrPin = Config::PIN_MODEM_PWR, 
               uint32_t baudRate = Config::MODEM_BAUD);

    bool isInitialized() const { return _initialized; }
};

#endif // CELLULAR_MANAGER_H
