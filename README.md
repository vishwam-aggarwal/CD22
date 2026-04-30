# CD22 Laser Distance Sensor — Arduino Library

Arduino library for the **CD22 series laser displacement sensors** by Wenglor / compatible OEMs. Communicates over **RS485** using a simple 6-byte binary protocol.

---

## Features

- Auto-detects connected sensor variant on startup
- Read distance as a calibrated `float` (mm) or raw `int16_t`
- Control laser power (on/off)
- Configure sample rate and averaging
- Set and release zero-point reference
- Configurable communication timeout

---

## Supported Sensors

| Model              | Measurement Range | Resolution |
|--------------------|:-----------------:|:----------:|
| CD22-15 (RS485)    | 15 mm             | 1 µm       |
| CD22-35 (RS485)    | 35 mm             | 10 µm      |
| CD22-100 (RS485)   | 100 mm            | 10 µm      |

---

## Requirements

**Arduino board with a hardware `Serial1` port** — e.g. Mega 2560, Leonardo, Due, Zero, ESP32, STM32. The library does **not** work on Uno or Nano (no `Serial1`).

**RS485 transceiver module with automatic direction switching.** The library does not manage a DE/RE direction-control pin, so the transceiver must handle TX/RX direction automatically (e.g. modules based on the SP3485 or similar auto-direction chips, or a module with DE and RE permanently tied together).

---

## Wiring

```
Arduino                RS485 Module            CD22 Sensor
─────────              ────────────            ───────────
Serial1 TX  ────────►  DI                      
Serial1 RX  ◄────────  RO                      
5V / 3.3V   ────────►  VCC                     
GND         ────────►  GND                     
                       A  ◄──────────────────► A
                       B  ◄──────────────────► B
```

> **Note:** Polarity of A/B matters. If the sensor does not respond, swap the A and B wires.

`Serial1` pin locations by board:

| Board          | Serial1 TX | Serial1 RX |
|----------------|:----------:|:----------:|
| Arduino Mega   | 18         | 19         |
| Arduino Due    | 18         | 19         |
| Arduino Leonardo | 1        | 0          |
| ESP32 (default)| 17         | 16         |

---

## Installation

### Arduino Library Manager (recommended)
1. Open the Arduino IDE.
2. Go to **Sketch → Include Library → Manage Libraries…**
3. Search for **CD22**.
4. Click **Install**.

### Manual Installation
1. Download this repository as a `.zip`.
2. In the Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library…**
3. Select the downloaded file.

---

## Quick Start

```cpp
#include "CD22.h"

CD22 sensor;

void setup() {
  Serial.begin(115200);

  CD22_SensorType type = sensor.begin(CD22_SERIAL_TYPE_HW, CD22_SERIAL_BAUD_115200);
  if (type == CD22_SENSOR_NO_SENSOR) {
    Serial.println("No sensor found. Check wiring.");
    while (true);
  }

  sensor.LaserON();
  delay(500); // allow laser to stabilise
}

void loop() {
  float distance = sensor.readDistance(); // millimetres
  Serial.println(distance);
  delay(100);
}
```

---

## API Reference

### Initialisation

```cpp
CD22_SensorType begin(
    CD22_SerialType  serialType = CD22_SERIAL_TYPE_HW,
    CD22_SerialBaud  baud       = CD22_SERIAL_BAUD_9600,
    uint8_t          timeoutMs  = 60
);
```

Initialises serial communication and auto-detects the sensor. Returns the detected sensor type or `CD22_SENSOR_NO_SENSOR` on failure. `timeoutMs` is the maximum time (0–255 ms) to wait for a sensor response.

Only `CD22_SERIAL_TYPE_HW` (hardware `Serial1`) is currently supported.

---

### Laser Control

```cpp
bool LaserON();    // turns the laser on
bool LaserOFF();   // turns the laser off
```

Returns `true` on success, `false` if the sensor did not acknowledge.

---

### Reading Distance

```cpp
float   readDistance();     // calibrated distance in millimetres
int16_t readDistanceRaw();  // raw sensor count
```

Both return `0` if communication fails.

---

### Sample Time

```cpp
CD22_SensorSampleTime getSampleTime();
bool setSampleTime(CD22_SensorSampleTime time = CD22_SENSOR_SAMPLE_TIME_AUTO);
```

| Constant                          | Period  |
|-----------------------------------|:-------:|
| `CD22_SENSOR_SAMPLE_TIME_500US`   | 500 µs  |
| `CD22_SENSOR_SAMPLE_TIME_1000US`  | 1 ms    |
| `CD22_SENSOR_SAMPLE_TIME_2000US`  | 2 ms    |
| `CD22_SENSOR_SAMPLE_TIME_4000US`  | 4 ms    |
| `CD22_SENSOR_SAMPLE_TIME_AUTO`    | auto    |

---

### Averaging

```cpp
CD22_SensorAvgNum getAveraging();
bool setAveraging(CD22_SensorAvgNum avg = CD22_SENSOR_AVG_NUM_1);
```

| Constant                  | Samples averaged |
|---------------------------|:----------------:|
| `CD22_SENSOR_AVG_NUM_1`   | 1                |
| `CD22_SENSOR_AVG_NUM_8`   | 8                |
| `CD22_SENSOR_AVG_NUM_64`  | 64               |
| `CD22_SENSOR_AVG_NUM_512` | 512              |

Higher averaging reduces noise at the cost of update rate.

---

### Zero Reference

```cpp
bool executeZeroReset();   // sets current position as zero
bool releaseZeroReset();   // clears the zero offset
```

---

## Baud Rates

Supported values for `CD22_SerialBaud`:

`9600` · `19200` · `38400` · `57600` · `115200`

---

## License

MIT — see [LICENSE](LICENSE).
