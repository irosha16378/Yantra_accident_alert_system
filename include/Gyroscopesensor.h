#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MotionDetector {
private:
    Adafruit_MPU6050 mpu;
    float vibrationThreshold;

public:
    // Constructor
    MotionDetector(float threshold) {
        vibrationThreshold = threshold;
    }

    // Initialize the sensor
    bool initSensor() {
        if (!mpu.begin()) {
            return false;
        }
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        return true;
    }

    // Check for sudden vibration or impact
    bool isVibrationDetected() {
        sensors_event_t accel, gyro, temp;
        mpu.getEvent(&accel, &gyro, &temp);

        float totalGyroChange = abs(gyro.gyro.x) + abs(gyro.gyro.y) + abs(gyro.gyro.z);

        Serial.print("Current Gyro Level: ");
        Serial.println(totalGyroChange);

        return (totalGyroChange > vibrationThreshold);
    }
};

#endif