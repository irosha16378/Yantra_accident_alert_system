#include <Arduino.h>
#include "Config.h"

void setup() {
    Serial.begin(Config::MONITOR_BAUD);
    delay(1000);
    Serial.println("=========================================");
    Serial.println(" Yantra Accident Alert System - Step 1 ");
    Serial.println("=========================================");
}

void loop() {
    // Step 1 logic will be built here
}