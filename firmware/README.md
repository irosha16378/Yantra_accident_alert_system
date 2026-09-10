# ESP32 Firmware Subsystem

This folder contains the **C++ PlatformIO / Arduino** firmware running on the ESP32 microcontroller for the **Yantra Motorcycle Accident Alert System**.

---

## 1. Directory Layout

```text
firmware/
├── include/                   # Header files
│   ├── Config.h               # Hardware pinout & system constants
│   ├── IMUSensor.h            # MPU6050 Motion Sensor driver header
│   ├── HandlebarSensor.h      # Dual handlebar hand touch sensor header
│   ├── FusionEngine.h         # Sensor fusion algorithm header
│   ├── GPSManager.h           # NMEA GPS module header
│   ├── CellularManager.h      # 4G SIM module header
│   └── SystemController.h     # Core system orchestrator
├── src/                       # Source files
│   ├── IMUSensor.cpp          # MPU6050 I2C register driver & math
│   ├── HandlebarSensor.cpp      # Hand grip detection logic
│   ├── FusionEngine.cpp       # Impact + hand presence evaluator
│   ├── GPSManager.cpp         # NMEA parser implementation
│   ├── CellularManager.cpp    # 4G AT command module implementation
│   ├── SystemController.cpp   # System state machine
│   └── main.cpp               # Firmware setup & execution loop
├── lib/                       # Third-party & custom platform libraries
├── test/                      # Unit tests
└── platformio.ini             # PlatformIO build & environment configuration
```

---

## 2. Part 1 Component: IMU Motion Sensor (`IMUSensor`)

### Hardware Connection & Pinout

| ESP32 Pin | MPU6050 Pin | Signal / Description |
| :---: | :---: | :--- |
| **GPIO 21** | **SDA** | $I^2C$ Data Line |
| **GPIO 22** | **SCL** | $I^2C$ Clock Line |
| **3.3V** | **VCC** | Power Supply (3.3V) |
| **GND** | **GND** | Ground |

---

### Key Driver Features

- **Device Address:** Default `0x68` (AD0 pin grounded).
- **Scale Configuration:**
  - Accelerometer: $\pm 8g$ ($4096 \text{ LSB}/g$)
  - Gyroscope: $\pm 250^\circ/s$ ($131.0 \text{ LSB}/(\text{deg}/s)$)
- **Data Filtering:** Exponential Moving Average (EMA) filter on G-Force magnitude ($|G| = \sqrt{A_x^2 + A_y^2 + A_z^2}$).
- **Attitude Angles:** Calculates **Pitch** and **Roll** angles in degrees using $I^2C$ accelerometer vectors.
- **Impact & Tumble Triggers:**
  - `isImpactDetected(thresholdG)`: Triggers when $|G| \ge \text{threshold}$ (default: $3.0g$).
  - `isTumbleDetected(thresholdDeg)`: Triggers when Pitch or Roll exceeds threshold (default: $60^\circ$).
- **Offset Calibration:** Automatic static calibration routine (`calibrate()`) to eliminate sensor drift.

---

### Code Usage Example

```cpp
#include <Arduino.h>
#include "Config.h"
#include "IMUSensor.h"

IMUSensor motionSensor;

void setup() {
    Serial.begin(Config::MONITOR_BAUD);
    
    // Initialize I2C and MPU6050
    if (motionSensor.begin(Config::PIN_SDA, Config::PIN_SCL)) {
        motionSensor.calibrate(100); // 100 sample calibration
    }
}

void loop() {
    motionSensor.update();

    if (motionSensor.isInitialized()) {
        const auto& data = motionSensor.getData();
        Serial.printf("Ax: %.2fg, Ay: %.2fg, Az: %.2fg | Total G: %.2fg | Pitch: %.1f deg\n",
                      data.accelX, data.accelY, data.accelZ, data.totalG, data.pitch);

        if (motionSensor.isImpactDetected(3.0f)) {
            Serial.println("[!] Impact Detected!");
        }
    }
    delay(200);
}
```

---

## 3. Building & Flashing

```bash
# Compile firmware
pio run --project-dir firmware/

# Upload to ESP32 board
pio run --target upload --project-dir firmware/

# Open Serial Monitor
pio device monitor --project-dir firmware/
```
