#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 is working!");
}

void loop() {
    Serial.println("Yantra Accident Alert System - ESP32");
    delay(1000);
}