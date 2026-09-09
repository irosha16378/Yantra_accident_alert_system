#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>

class GPSManager {
public:
    GPSManager();
    bool begin();
    void update();
};

#endif // GPS_MANAGER_H

