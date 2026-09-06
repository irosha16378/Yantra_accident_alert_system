#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"
#include "HandlebarSensor.h"
#include "GPSManager.h"
#include "CellularManager.h"
#include "FusionEngine.h"

class SystemController {
private:
    IMUSensor _imu;
    HandlebarSensor _handlebar;
    GPSManager _gps;
    CellularManager _cellular;
    FusionEngine _fusionEngine;

    uint32_t _lastHeartbeat;

public:
    SystemController();

    void begin();
    void update();
};

#endif // SYSTEM_CONTROLLER_H
