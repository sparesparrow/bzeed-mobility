# ESP32 Multi (Wi‑Fi + HTTPS + BLE + MQTT + TinyGPS++)

Modular PlatformIO project for ESP32 featuring:
- Wi‑Fi manager
- MQTT client (PubSubClient) with a decoupled command handler
- HTTPS client (WiFiClientSecure)
- BLE service skeleton (NimBLE-Arduino)
- GPS via TinyGPS++
- Periodic status publish stub (JSON)
- **Scooter control** (`lib/scooter_control`): portable ES200-B ESC protocol
  codec + a reliability/redundancy layer (keep-alive, link watchdog, failsafe,
  redundant transports). Fully host-unit-tested — see below.

## Setup

1. Copy `include/secrets.h.example` to `include/secrets.h` and fill in your credentials.
2. Adjust pins in `include/config_gps.h` if needed.
3. Build and upload with PlatformIO.

## Topics

- Status publishes to `devices/esp32multi/status`.

## Scooter control commands

The MQTT command topic (`devices/esp32multi/cmd`) accepts:
`RUN`/`UNLOCK`, `STOP`/`LOCK`, `PING` (keep-alive). Each valid command feeds the
link watchdog; if commands stop arriving while running, the controller drives a
failsafe stop. See `lib/scooter_control/README.md` for the protocol and design.

## Tests

Host-side unit tests (no hardware / PlatformIO required):

```bash
./test/run_tests.sh
```

## Notes

- HTTPS uses root CA in `HTTPS_ROOT_CA` if provided; otherwise it uses insecure mode (not recommended for production).