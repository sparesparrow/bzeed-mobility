# ESP32 Firmware (PlatformIO)

```mermaid
flowchart LR
  Main[main.cpp loop] --> GPS[TinyGPS++]
  Main --> BLE[NimBLE]
  Main --> HTTP[WiFiClientSecure]
  Main --> MQTT[MQTT klient]
  MQTT --> Pub[Publikace statusu]
  HTTP --> REST[HTTPS volání]
```

## Příklad telemetrie
```json
{
  "scooter_id": "abc-123",
  "battery": 83,
  "location": {"lat": 49.277, "lng": 16.998},
  "ts": 1712345678
}
```
