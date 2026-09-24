// scooter_protocol.h
//
// Portable, dependency-free codec for the Okai / Bird "ES200-B" electric
// scooter ESC serial protocol. Contains no Arduino or ESP-IDF headers so it
// can be compiled and unit-tested on a host machine (see test/).
//
// Wire format (6 bytes, little-endian on the ESC UART @ 9600 8N1):
//
//   [0] 0xA6   frame preamble
//   [1] 0x12   source / addressing byte (host -> ESC)
//   [2] 0x02   payload length (command byte + parameter byte)
//   [3] cmd    command bit-field (see CommandBits below)
//   [4] speed  speed limit parameter (km/h), 0..kMaxSpeedKph
//   [5] crc    CRC-8/MAXIM over bytes [0..4]
//
// The constant frame header and the command bit-field layout are hardware
// facts of the ES200-B ESC. They were rediscovered from the community
// projects credited in the module README; this is a clean-room
// re-implementation, not a copy of their source.
//
// Design goals: small surface, no dynamic allocation, no I/O. Encoding and
// validation are pure functions so the reliability layer on top of them
// (ScooterController) stays fully testable without hardware.

#ifndef BZEED_SCOOTER_PROTOCOL_H
#define BZEED_SCOOTER_PROTOCOL_H

#include <cstddef>
#include <cstdint>

namespace bzeed {
namespace scooter {

// Fixed frame geometry.
constexpr std::size_t kFrameSize = 6;      // total bytes on the wire
constexpr std::size_t kCrcSpan = 5;        // bytes covered by the CRC
constexpr std::uint8_t kPreamble = 0xA6;   // frame[0]
constexpr std::uint8_t kAddress = 0x12;    // frame[1]
constexpr std::uint8_t kPayloadLen = 0x02; // frame[2]

// Speed parameter bounds (km/h). The ESC ignores obviously out-of-range
// values; we clamp so a caller mistake can never widen the limit.
constexpr std::uint8_t kMaxSpeedKph = 27;
constexpr std::uint8_t kDefaultSpeedKph = 20;

// Bit positions inside the command byte (frame[3]). Matches the ES200-B
// layout: bit 0 = power, then blink, light, (reserved), km/h, fast accel.
enum CommandBits : std::uint8_t {
  kBitPowerOn = 0,
  kBitLightBlink = 1,
  kBitLightOn = 2,
  // bit 3 reserved / always 0
  kBitUnitsKph = 4,
  kBitFastAccel = 5,
  // bits 6,7 reserved / always 0
};

// Human-readable command intent. Assembled into the wire command byte by
// makeCommandByte(); kept as a struct so call sites read declaratively
// instead of juggling raw booleans in positional order.
struct Command {
  bool power_on = false;
  bool light_on = false;
  bool light_blink = false;
  bool units_kph = true;    // display in km/h (Europe default) vs mph
  bool fast_accel = false;  // aggressive acceleration curve

  // Convenience constructors for the two dominant states.
  static Command run(bool light_on = true, bool fast_accel = false) {
    Command c;
    c.power_on = true;
    c.light_on = light_on;
    c.fast_accel = fast_accel;
    return c;
  }
  static Command stop(bool light_blink = false) {
    Command c;
    c.power_on = false;
    c.light_blink = light_blink;
    return c;
  }
};

// CRC-8/MAXIM (poly 0x31 reflected -> 0x8C, init 0x00). This is the checksum
// the ES200-B ESC expects; implemented from the standard definition rather
// than pulled from a CRC library so the module has zero dependencies.
std::uint8_t crc8Maxim(const std::uint8_t* data, std::size_t len);

// Build the command byte from a Command. Reserved bits are always zero.
std::uint8_t makeCommandByte(const Command& cmd);

// Encode a command + speed into a wire frame. `out` must have room for
// kFrameSize bytes. Speed is clamped to [0, kMaxSpeedKph]. Returns the number
// of bytes written (always kFrameSize) so it can be passed straight to a
// UART write call.
std::size_t encodeFrame(const Command& cmd, std::uint8_t speed_kph,
                        std::uint8_t out[kFrameSize]);

// Validate a received/echoed frame: correct length, header bytes and CRC.
// Pure predicate, safe on attacker-controlled buffers (bounds-checked).
bool isValidFrame(const std::uint8_t* frame, std::size_t len);

}  // namespace scooter
}  // namespace bzeed

#endif  // BZEED_SCOOTER_PROTOCOL_H
