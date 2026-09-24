# scooter_control

Portable C++ library for controlling an Okai/Bird **ES200-B** scooter ESC over
its serial bus, with a reliability and redundancy layer on top.

The design goal is **modularity + testability**: the protocol codec and the
control state machine contain no Arduino/ESP-IDF headers and read no clock of
their own, so the whole thing is exercised by host unit tests without any
hardware. The Arduino-specific glue (a `HardwareSerial` transport) lives in the
firmware's `src/` folder, not here.

## Layout

| File | Responsibility | Deps |
|------|----------------|------|
| `scooter_protocol.h/.cpp` | ES200-B frame codec: CRC-8/MAXIM, command bit-field, encode/validate | none |
| `reliability.h` | `Interval`, `Backoff`, `Watchdog` — generic timed-behaviour helpers | none |
| `scooter_controller.h/.cpp` | State machine: keep-alive, link watchdog, failsafe, redundant transports | the two above |

## Reliability & redundancy features

- **Keep-alive** — the ESC re-locks unless it keeps receiving valid *run*
  frames; the controller refreshes them at a fixed cadence.
- **Link watchdog** — if the authorised controller goes silent while the
  scooter is running, it is driven to a **failsafe stop** instead of coasting
  on stale authorisation.
- **Redundant transports** — up to two output links (e.g. primary ESC UART +
  a secondary/relayed path). A frame is delivered over the first healthy link;
  on failure the next is tried. A single link fault does not drop control.
- **Exponential backoff** when every transport is failing.

## Wire format

6-byte frame at 9600 8N1:

```
[0] 0xA6  preamble
[1] 0x12  address (host -> ESC)
[2] 0x02  payload length
[3] cmd   command bit-field (bit0 power, bit1 blink, bit2 light, bit4 km/h, bit5 fast-accel)
[4] speed km/h limit (clamped)
[5] crc   CRC-8/MAXIM over [0..4]
```

## Tests

```bash
./esp32-multi/test/run_tests.sh   # builds with g++, no hardware needed
```

Also run in CI by `.github/workflows/esp32-tests.yml`. The golden CRC/frame
vectors are real frames captured from ES200-B scooters.

## Credits

The ES200-B wire format (frame header, command bit-field, CRC choice and
keep-alive behaviour) was rediscovered by these community projects. This
library is a clean-room re-implementation of those **hardware facts**, not a
copy of their source:

- [iAlexander/ES-UNLOCKER](https://github.com/iAlexander/ES-UNLOCKER) (MIT)
- [chappy1978/ES200-Scooter-Unlocker](https://github.com/chappy1978/ES200-Scooter-Unlocker) (GPLv3)
- [l064n/ScooterStuff](https://github.com/l064n/ScooterStuff)
