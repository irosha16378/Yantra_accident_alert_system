#ifndef FUSION_ENGINE_H
#define FUSION_ENGINE_H

#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"
#include "HandlebarSensor.h"
#include "GPSManager.h"
#include "CellularManager.h"

class FusionEngine {
private:
    IMUSensor& _imu;
    HandlebarSensor& _handlebar;
    GPSManager& _gps;
    CellularManager& _cellular;

    Config::SystemState _state;
    uint32_t _countdownStartTime;

    void handleMonitoring();
    void handleCountdown();

public:
    FusionEngine(IMUSensor& imu,
                 HandlebarSensor& handlebar,
                 GPSManager& gps,
                 CellularManager& cellular);

    void begin();
    void update();
    void cancelAlert();
    Config::SystemState getState() const;
};

#endif // FUSION_ENGINE_H
