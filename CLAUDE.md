# CD22 Arduino Library

Arduino library for the CD22 laser distance sensor family, communicating over RS485 via `Serial1` (hardware serial).

## Sensor Variants

| Enum                   | Value  | Range   | Least Count |
|------------------------|--------|---------|-------------|
| `CD22_SENSOR_15_485`   | `0x0f` | 15 mm   | 1 µm        |
| `CD22_SENSOR_35_485`   | `0x23` | 35 mm   | 10 µm       |
| `CD22_SENSOR_100_485`  | `0x64` | 100 mm  | 10 µm       |

`begin()` auto-detects the connected variant by reading the model register and returns the matching enum, or `CD22_SENSOR_NO_SENSOR` on failure.

## Protocol

6-byte binary frame over RS485:

```
[STX(0x02), type, data1, data2, ETX(0x03), XOR(type^data1^data2)]
```

- **Command types:** `0x52` (READ), `0x57` (WRITE), `0x43` (parameter/control operations)
- **Response type byte** is always `ACK (0x06)` on success; `readData()` validates STX, ETX, ACK, and XOR checksum
- Timeout is configurable in `begin()` (default 60 ms); `readData()` blocks until 6 bytes arrive or timeout expires

## Distance Output

`readDistance()` returns a `float` in **millimetres**:
```
distance_mm = raw_int16 × (least_count_µm / 1000.0)
```
Returns `0.0` on failure — callers must handle the ambiguity since 0 can be a valid measurement near the zero point.

`readDistanceRaw()` returns the signed raw count directly.

## Hardware Assumptions

- **Hardcoded to `Serial1`** — no way to select a different port without editing the source.
- **RS485 direction pin not managed** — the library assumes the RS485 transceiver auto-switches direction (common on modules with automatic DE/RE). Manual half-duplex direction control is not implemented.
- `CD22_SERIAL_TYPE_SW` (software serial) is defined in the enum but **not implemented** — passing it to `begin()` always returns `CD22_SENSOR_NO_SENSOR`.

## Known Issues

### 1. `readDistanceRaw` is unreachable as a class method (`CD22.cpp:124`)

```cpp
int16_t readDistanceRaw(void)   // missing CD22:: — compiled as a free function
```

The method is declared in the header as a class member, but defined in the .cpp without the `CD22::` scope qualifier, making it a free function with different linkage. The current example never calls it, so this doesn't surface. Any attempt to call `sensor.readDistanceRaw()` will produce a linker error.

Fix: add `CD22::` to the definition.

### 2. `writeData` returns nothing despite `bool` return type (`CD22.cpp:80`)

The function signature is `bool CD22::writeData(...)` but there is no `return` statement. Undefined behaviour in C++, though harmless in current usage since no caller checks the return value.

Fix: change return type to `void`, or add `return true`.

### 3. `getSampleTime` and `getAveraging` have no default return

If the switch statement matches no case (unexpected sensor response), the function falls off the end without returning a value — undefined behaviour. Works in practice as long as the sensor behaves correctly.

Fix: add a `default:` case to each switch.

### 4. Misleading enum comments on `CD22_SerialType` (`CD22.h:39–42`)

The comments describe alert-latch behaviour from an unrelated driver. They should simply say "Hardware serial (Serial1)" and "Software serial (not implemented)".

## Model Detection Logic

`begin()` decodes the sensor type as `~data1 & data2` on the model register response bytes — bits that are 0 in `data1` and 1 in `data2`. The result maps to the sensor type constants (0x0f, 0x23, 0x64).

## setSampleTime / setAveraging Pattern

Both follow a read-then-write sequence:
1. Send READ command to the target register
2. Wait for ACK
3. Send WRITE command with `data1=0x00` and `data2=<new_value>` (register implied by the preceding read)
4. Verify by calling `get*()` again

## File Structure

```
CD22.h                            — class declaration, enums, register/protocol constants
CD22.cpp                          — class implementation
examples/CD22_Test/CD22_Test.ino  — usage example (setup-only, loop is empty)
```
