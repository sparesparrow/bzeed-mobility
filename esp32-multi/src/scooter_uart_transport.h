#pragma once

// Arduino HardwareSerial implementation of scooter::ITransport.
//
// Lives under src/ so it is only compiled by PlatformIO for the ESP32 target;
// the portable library in lib/scooter_control has no Arduino dependency and is
// what the host unit tests link against.

#include <Arduino.h>

#include "scooter_controller.h"

class ScooterUartTransport : public bzeed::scooter::ITransport {
 public:
  // `enable_pin` (optional, -1 to disable) drives a transistor/relay that
  // must be held while talking to the ESC on some wiring harnesses.
  ScooterUartTransport(HardwareSerial& serial, const char* name,
                       int enable_pin = -1)
      : serial_(serial), name_(name), enable_pin_(enable_pin) {}

  void begin(unsigned long baud, int rx_pin, int tx_pin) {
    if (enable_pin_ >= 0) {
      pinMode(enable_pin_, OUTPUT);
      digitalWrite(enable_pin_, HIGH);
    }
    serial_.begin(baud, SERIAL_8N1, rx_pin, tx_pin);
    ready_ = true;
  }

  bool send(const std::uint8_t* data, std::size_t len) override {
    if (!ready_) return false;
    const size_t written = serial_.write(data, len);
    return written == len;
  }

  bool healthy() const override { return ready_; }

  const char* name() const override { return name_; }

 private:
  HardwareSerial& serial_;
  const char* name_;
  int enable_pin_;
  bool ready_ = false;
};
