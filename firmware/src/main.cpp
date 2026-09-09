#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"

IMUSensor motionSensor;

void setup() {
    Serial.begin(Config::MONITOR_BAUD);
    delay(1000);
    Serial.println("==========================================");
    Serial.println(" STEP 1: ESP32 + Motion Sensor (MPU6050)");
    Serial.println("==========================================");

    if (motionSensor.begin()) {
        Serial.println("[SUCCESS] Motion sensor initialized successfully!");
    } else {
        Serial.println("[WARNING] Could not find MPU6050. Please check wiring (SDA=GPIO21, SCL=GPIO22, VCC=3.3V, GND=GND).");
    }
}

void loop() {
    motionSensor.update();

    if (motionSensor.isInitialized()) {
        Serial.print("Accel X: ");
        Serial.print(motionSensor.getGX(), 2);
        Serial.print(" g | Y: ");
        Serial.print(motionSensor.getGY(), 2);
        Serial.print(" g | Z: ");
        Serial.print(motionSensor.getGZ(), 2);
        Serial.print(" g | Total Magnitude: ");
        Serial.print(motionSensor.getTotalGForce(), 2);
        Serial.println(" g");
    }

    delay(500); // Read sample every 500ms
}