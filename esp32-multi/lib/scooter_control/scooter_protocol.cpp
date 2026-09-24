// scooter_protocol.cpp — see scooter_protocol.h for the wire format.

#include "scooter_protocol.h"

namespace bzeed {
namespace scooter {

std::uint8_t crc8Maxim(const std::uint8_t* data, std::size_t len) {
  // CRC-8/MAXIM: reflected polynomial 0x8C, initial value 0x00.
  std::uint8_t crc = 0x00;
  for (std::size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (std::uint8_t bit = 0; bit < 8; ++bit) {
      if (crc & 0x01) {
        crc = static_cast<std::uint8_t>((crc >> 1) ^ 0x8C);
      } else {
        crc = static_cast<std::uint8_t>(crc >> 1);
      }
    }
  }
  return crc;
}

std::uint8_t makeCommandByte(const Command& cmd) {
  std::uint8_t b = 0;
  if (cmd.power_on) b |= (1u << kBitPowerOn);
  if (cmd.light_blink) b |= (1u << kBitLightBlink);
  if (cmd.light_on) b |= (1u << kBitLightOn);
  if (cmd.units_kph) b |= (1u << kBitUnitsKph);
  if (cmd.fast_accel) b |= (1u << kBitFastAccel);
  return b;
}

std::size_t encodeFrame(const Command& cmd, std::uint8_t speed_kph,
                        std::uint8_t out[kFrameSize]) {
  if (speed_kph > kMaxSpeedKph) speed_kph = kMaxSpeedKph;
  out[0] = kPreamble;
  out[1] = kAddress;
  out[2] = kPayloadLen;
  out[3] = makeCommandByte(cmd);
  out[4] = speed_kph;
  out[5] = crc8Maxim(out, kCrcSpan);
  return kFrameSize;
}

bool isValidFrame(const std::uint8_t* frame, std::size_t len) {
  if (frame == nullptr || len != kFrameSize) return false;
  if (frame[0] != kPreamble || frame[1] != kAddress || frame[2] != kPayloadLen) {
    return false;
  }
  return frame[5] == crc8Maxim(frame, kCrcSpan);
}

}  // namespace scooter
}  // namespace bzeed
