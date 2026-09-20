#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"

// Instantiate the MPU6050 Motion Sensor object
IMUSensor motionSensor;

void setup() {
    Serial.begin(Config::MONITOR_BAUD);
    delay(1000);
    
    Serial.println("=================================================");
    Serial.println(" STEP 1: ESP32 + MPU6050 Accelerometer & Gyro ");
    Serial.println("=================================================");
    Serial.println("Wiring Check:");
    Serial.println(" - VCC -> ESP32 3.3V (or 5V if module has 3.3V regulator)");
    Serial.println(" - GND -> ESP32 GND");
    Serial.println(" - SDA -> ESP32 GPIO21");
    Serial.println(" - SCL -> ESP32 GPIO22");
    Serial.println("-------------------------------------------------");

    // Initialize I2C and sensor
    if (motionSensor.begin(Config::PIN_SDA, Config::PIN_SCL)) {
        Serial.println("[SUCCESS] MPU6050 connected and initialized!");
        
        Serial.println("[INFO] Calibrating offsets... Keep module completely level and still.");
        motionSensor.calibrate(100);
        Serial.println("[SUCCESS] Sensor ready for live measurements.");
    } else {
        Serial.println("[ERROR] MPU6050 not detected. Please verify wiring & connections.");
    }
}

void loop() {
    // Read fresh sensor data
    motionSensor.update();

    if (motionSensor.isInitialized()) {
        const auto& data = motionSensor.getData();

        // Print Accelerometer Data (g-force)
        Serial.print("Accel [X: "); Serial.print(data.accelX, 2);
        Serial.print("g, Y: "); Serial.print(data.accelY, 2);
        Serial.print("g, Z: "); Serial.print(data.accelZ, 2);
        Serial.print("g] | Mag: "); Serial.print(data.totalG, 2);

        // Print Tilt Angles (Pitch & Roll)
        Serial.print("g | Tilt [Pitch: "); Serial.print(data.pitch, 1);
        Serial.print(" deg, Roll: "); Serial.print(data.roll, 1);
        Serial.print(" deg] | Temp: "); Serial.print(data.temperature, 1);
        Serial.print(" C");

        // Event Detection Flags
        if (motionSensor.isImpactDetected(3.0f)) {
            Serial.print(" ---> [!] HIGH IMPACT DETECTED!");
        }
        if (motionSensor.isTumbleDetected(60.0f)) {
            Serial.print(" ---> [!] ACCIDENT TUMBLE/FALL DETECTED!");
        }
        Serial.println();
    }

    delay(200); // 5Hz output frequency for Serial Monitor
}