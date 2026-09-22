#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"

// Instantiate Gyroscope / IMU sensor driver object
IMUSensor imu;

unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL_MS = 200; // Print every 200ms (5Hz)

void setup() {
    // Initialize Serial Monitor for interactive output
    Serial.begin(Config::MONITOR_BAUD);
    delay(1000); // Allow hardware serial connection to stabilize

    Serial.println();
    Serial.println("==================================================");
    Serial.println("   Yantra System - Step 1: MPU6050 Gyro Sensor Test");
    Serial.println("==================================================");
    Serial.printf("Connecting to MPU6050 on SDA=GPIO%d, SCL=GPIO%d...\n", 
                  Config::PIN_SDA, Config::PIN_SCL);

    // Initialize MPU6050 I2C connection
    if (imu.begin(Config::PIN_SDA, Config::PIN_SCL, Config::MPU6050_I2C_ADDR)) {
        Serial.println("[SUCCESS] MPU6050 Initialized!");
        
        // Calibrate static Gyroscope offsets (Keep sensor still during startup)
        Serial.println("Keep sensor STILL on flat surface during calibration...");
        imu.calibrate(100);
        
        Serial.println("Calibration complete! Live readings starting below:");
        Serial.println("------------------------------------------------------------------");
        Serial.println(" Gyro X | Gyro Y | Gyro Z | Pitch | Roll  | Total G | Status");
        Serial.println(" (deg/s)| (deg/s)| (deg/s)| (deg) | (deg) |   (g)   | ");
        Serial.println("------------------------------------------------------------------");
    } else {
        Serial.println("[ERROR] Failed to find MPU6050! Check your SDA/SCL wiring & VCC (3.3V).");
    }
}

void loop() {
    // Update IMU measurements continuously
    imu.update();

    // Periodically print readings to Serial Monitor
    if (millis() - lastPrintTime >= PRINT_INTERVAL_MS) {
        lastPrintTime = millis();

        if (imu.isInitialized()) {
            const auto& data = imu.getData();

            // Check triggers
            bool impact = imu.isImpactDetected(Config::ACCIDENT_IMPACT_THRESHOLD_G);
            bool tumble = imu.isTumbleDetected(Config::TUMBLE_ANGLE_THRESHOLD_DEG);

            String statusStr = "NORMAL";
            if (impact && tumble) {
                statusStr = "IMPACT + TUMBLE!";
            } else if (impact) {
                statusStr = "IMPACT DETECTED!";
            } else if (tumble) {
                statusStr = "TUMBLE TILTED!";
            }

            Serial.printf(" %6.1f | %6.1f | %6.1f | %5.1f | %5.1f | %7.2f | %s\n",
                          data.gyroX, data.gyroY, data.gyroZ,
                          data.pitch, data.roll, data.totalG,
                          statusStr.c_str());
        } else {
            Serial.println("[ERROR] MPU6050 sensor offline.");
        }
    }
}