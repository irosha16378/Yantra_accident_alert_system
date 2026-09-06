# Yantra Accident Alert System

An intelligent IoT-based accident detection and alert system featuring ESP32 firmware integration and a dedicated PC dashboard application.

---

## 📁 Repository Structure

```text
Yantra_accident_alert_system/
├── firmware/              # ESP32 Firmware (PlatformIO / Arduino)
│   ├── src/               # Firmware source code (main.cpp)
│   ├── include/           # Header files
│   ├── lib/               # Custom libraries & drivers
│   ├── test/              # Firmware test scripts
│   ├── config/            # Firmware configurations
│   ├── drivers/           # Hardware sensor drivers
│   └── platformio.ini     # PlatformIO configuration
├── pc_app/                # PC Application & Dashboard
│   ├── src/               # Source code
│   │   ├── ui/            # Dashboard user interface
│   │   ├── core/          # Core business & processing logic
│   │   ├── utils/         # Utility scripts & helpers
│   │   └── api/           # Communication & API handlers
│   ├── assets/            # UI assets and static media
│   ├── config/            # Application settings
│   ├── docs/              # Dashboard documentation
│   ├── tests/             # Application test suite
│   ├── main.py            # PC application entry point
│   └── requirements.txt   # Python dependencies manifest
└── docs/                  # System documentation
    ├── hardware/          # Hardware schematics & wiring diagrams
    ├── software/          # Software design & specifications
    ├── images/            # Project diagrams & screenshots
    └── testing/           # Test plans & logs
```

---

## 🚀 Getting Started

### 1. Firmware Setup (ESP32)
The firmware is built using [PlatformIO](https://platformio.org/).

- **Board**: ESP32 Dev Module (`esp32dev`)
- **Framework**: Arduino
- **Baud Rate**: `115200`

#### Building & Flashing Firmware
1. Open the project in VS Code with the PlatformIO extension installed.
2. Connect your ESP32 board via USB.
3. Build and upload:
   ```bash
   # Using PlatformIO CLI inside firmware/ directory
   pio run --target upload
   ```
4. Monitor serial output:
   ```bash
   pio device monitor
   ```

### 2. PC Dashboard Application
The PC application located in `pc_app/` connects with the accident alert hardware to display real-time telemetry, location data, and alert notifications.

---

## 🛠️ Tech Stack
- **Embedded Firmware**: C++ / Arduino Framework / ESP32
- **Build System**: PlatformIO
- **PC Dashboard**: Python GUI / Desktop Application Framework

---

## 📄 License
This project is developed for the Yantra Accident Alert System.
