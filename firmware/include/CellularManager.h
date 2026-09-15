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
    bool _registered;

    void clearBuffer();

public:
    explicit CellularManager(HardwareSerial& serial = Serial1);

    /**
     * @brief Initialize cellular hardware UART and modem pins
     */
    bool begin(int rxPin = Config::PIN_MODEM_RX, 
               int txPin = Config::PIN_MODEM_TX, 
               int pwrPin = Config::PIN_MODEM_PWR, 
               uint32_t baudRate = Config::MODEM_BAUD);

    /**
     * @brief Power on the modem using power key / reset GPIO pin sequence
     */
    void powerOn();

    /**
     * @brief Hard reset the modem module
     */
    void reset();

    /**
     * @brief Send raw AT command and wait for expected substring response
     */
    bool sendATCommand(const String& cmd, const char* expectedReply = "OK", uint32_t timeoutMs = 2000);

    /**
     * @brief Send raw AT command and capture the text output response
     */
    bool sendATCommandWithResponse(const String& cmd, String& response, uint32_t timeoutMs = 2000);

    /**
     * @brief Test basic AT communication with modem
     */
    bool checkModemResponse(uint32_t timeoutMs = 1000);

    /**
     * @brief Check if SIM card is inserted and ready (CPIN: READY)
     */
    bool checkSIMReady();

    /**
     * @brief Check cellular network registration status (CREG / CGREG / CEREG)
     */
    bool checkNetworkRegistration();

    // Getters
    bool isInitialized() const { return _initialized; }
    bool isRegistered() const { return _registered; }
};

#endif // CELLULAR_MANAGER_H
