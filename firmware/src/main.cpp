#include <Arduino.h>
#include "SystemController.h"

// Instantiate System Controller instance using OOP Encapsulation
static SystemController app;

void setup() {
    app.begin();
}

void loop() {
    app.update();
}