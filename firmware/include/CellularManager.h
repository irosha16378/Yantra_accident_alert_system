#ifndef CELLULAR_MANAGER_H
#define CELLULAR_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class CellularManager {
private:
    bool _isInitialized;

public:
    CellularManager();
    bool begin();
    bool sendEmergencyAlert(const Config::LocationData& location);
};

#endif // CELLULAR_MANAGER_H
