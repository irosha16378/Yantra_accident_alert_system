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

    if (motionSensor.begin(Config::PIN_SDA, Config::PIN_SCL)) {
        Serial.println("[SUCCESS] MPU6050 Motion sensor initialized!");
        motionSensor.calibrate(100);
    } else {
        Serial.println("[WARNING] Could not find MPU6050. Check wiring (SDA=GPIO21, SCL=GPIO22, VCC=3.3V, GND=GND).");
    }
}

void loop() {
    motionSensor.update();

    if (motionSensor.isInitialized()) {
        const auto& data = motionSensor.getData();

        Serial.print("Ax: "); Serial.print(data.accelX, 2);
        Serial.print("g | Ay: "); Serial.print(data.accelY, 2);
        Serial.print("g | Az: "); Serial.print(data.accelZ, 2);
        Serial.print("g | Mag: "); Serial.print(data.totalG, 2);
        Serial.print("g (Flt: "); Serial.print(data.filteredG, 2);
        Serial.print("g) | Pitch: "); Serial.print(data.pitch, 1);
        Serial.print(" deg | Roll: "); Serial.print(data.roll, 1);
        Serial.print(" deg | Temp: "); Serial.print(data.temperature, 1);
        Serial.print(" C");

        if (motionSensor.isImpactDetected(3.0f)) {
            Serial.print(" | [!] HIGH IMPACT");
        }
        if (motionSensor.isTumbleDetected(60.0f)) {
            Serial.print(" | [!] TUMBLE/FALL DETECTED");
        }
        Serial.println();
    }

    delay(200); // Sample every 200ms
}