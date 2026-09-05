#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 is working!");
}

void loop() {
    Serial.println("Hello from ESP32");
    delay(1000);
}