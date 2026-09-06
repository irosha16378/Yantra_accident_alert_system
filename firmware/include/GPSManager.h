#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class GPSManager {
private:
    Config::LocationData _currentLocation;
    bool _isInitialized;

public:
    GPSManager();
    bool begin();
    void update();
    Config::LocationData getLocation() const;
    bool hasValidFix() const;
};

#endif // GPS_MANAGER_H
