# ESP32 OBD-II BLE Bridge (for Android Assistant)

This firmware turns an ESP32 into a read-only OBD-II bridge over BLE (Nordic UART Service).
- Direct CAN via ESP32 TWAI + transceiver (SN65HVD230/TJA1050)
- BLE service UUID: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
  - RX (phone→ESP32) UUID: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (Write/WriteNR)
  - TX (ESP32→phone) UUID: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (Notify)
- Notifies newline-delimited JSON: `{"t":"telemetry","rpm":2310,"spd_kph":88,"coolant_c":82}`

## Wiring
- OBD-II pin 6 → CANH, pin 14 → CANL
- Transceiver TXD → ESP32 GPIO 21, RXD → GPIO 22
- GND common; no additional 120Ω termination

## Build and flash

Install PlatformIO and build:
```bash
pio run -d firmware/esp32-obd-ble-can
```
Flash (adjust serial path if needed):
```bash
pio run -d firmware/esp32-obd-ble-can -t upload
```
The device advertises as `ESP32-OBD`.

## Android App integration
- Connect to the NUS service, enable notifications on TX.
- Parse lines as JSON; update your assistant state.
- Send commands (optional) to RX for configuring PID list or period.

## Safety
- Read-only OBD PID requests (0x7DF). Do not send non-OBD frames.
- Use an inline fuse and ACC-switched power.
