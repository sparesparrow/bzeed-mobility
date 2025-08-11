# ESP32 Multi (Wi‑Fi + HTTPS + BLE + MQTT + TinyGPS++)

Modular PlatformIO project for ESP32 featuring:
- Wi‑Fi manager
- MQTT client (PubSubClient)
- HTTPS client (WiFiClientSecure)
- BLE service skeleton (NimBLE-Arduino)
- GPS via TinyGPS++
- Periodic status publish stub (JSON)

## Setup

1. Copy `include/secrets.h.example` to `include/secrets.h` and fill in your credentials.
2. Adjust pins in `include/config_gps.h` if needed.
3. Build and upload with PlatformIO.

## Topics

- Status publishes to `devices/esp32multi/status`.

## Notes

- HTTPS uses root CA in `HTTPS_ROOT_CA` if provided; otherwise it uses insecure mode (not recommended for production).