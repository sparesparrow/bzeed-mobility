// reliability.h
//
// Small, header-only, dependency-free building blocks for reliable timed
// behaviour on a single-threaded embedded loop. All classes take time as an
// explicit millisecond argument (`now_ms`) rather than calling millis()
// directly, so they contain no hidden global state and are unit-testable on
// a host with a fake clock.
//
// These are reused by ScooterController but are deliberately generic; the
// existing Wi-Fi/MQTT managers can adopt Backoff to replace their ad-hoc
// fixed reconnect delays.

#ifndef BZEED_RELIABILITY_H
#define BZEED_RELIABILITY_H

#include <cstdint>

namespace bzeed {

// Monotonic milliseconds. Kept as a typedef so the intent is explicit at
// call sites and the width is fixed across host and target.
using Millis = std::uint32_t;

// Fires at a fixed cadence. due(now) returns true at most once per period and
// arms the next deadline relative to the fire time, so periods do not drift.
class Interval {
 public:
  explicit Interval(Millis period_ms) : period_ms_(period_ms) {}

  void setPeriod(Millis period_ms) { period_ms_ = period_ms; }

  bool due(Millis now_ms) {
    if (!armed_) {
      armed_ = true;
      last_ms_ = now_ms;
      return true;  // fire immediately on first use
    }
    if (now_ms - last_ms_ >= period_ms_) {
      last_ms_ = now_ms;
      return true;
    }
    return false;
  }

  // Re-arm so the next due() fires immediately, then resumes the cadence.
  // Used when a state change should emit a frame right away.
  void reset(Millis /*now_ms*/) { armed_ = false; }

 private:
  Millis period_ms_;
  Millis last_ms_ = 0;
  bool armed_ = false;
};

// Exponential backoff with a ceiling, for retrying a failing operation
// (connect, publish) without hammering. ready(now) is true once the current
// delay has elapsed since the last attempt; call recordAttempt() when you try
// and reset() on success.
class Backoff {
 public:
  Backoff(Millis base_ms, Millis max_ms)
      : base_ms_(base_ms), max_ms_(max_ms), current_ms_(base_ms) {}

  // True when enough time has passed to attempt again.
  bool ready(Millis now_ms) const {
    return !attempted_ || (now_ms - last_attempt_ms_ >= current_ms_);
  }

  // Mark that an attempt was just made and grow the delay for next time.
  void recordAttempt(Millis now_ms) {
    attempted_ = true;
    last_attempt_ms_ = now_ms;
    Millis next = current_ms_ * 2;
    current_ms_ = (next > max_ms_ || next < current_ms_) ? max_ms_ : next;
  }

  // Call on success: collapse back to the base delay.
  void reset() {
    current_ms_ = base_ms_;
    attempted_ = false;
  }

  Millis currentDelay() const { return current_ms_; }

 private:
  Millis base_ms_;
  Millis max_ms_;
  Millis current_ms_;
  Millis last_attempt_ms_ = 0;
  bool attempted_ = false;
};

// Liveness watchdog. Something must call feed() within timeout_ms of the last
// feed, or expired(now) becomes true. Used to detect a dead link and drop the
// scooter into a failsafe state.
class Watchdog {
 public:
  explicit Watchdog(Millis timeout_ms) : timeout_ms_(timeout_ms) {}

  void feed(Millis now_ms) {
    fed_ = true;
    last_feed_ms_ = now_ms;
  }

  bool expired(Millis now_ms) const {
    return fed_ && (now_ms - last_feed_ms_ >= timeout_ms_);
  }

  // A never-fed watchdog is treated as "not yet alive" rather than expired,
  // so startup does not immediately trip the failsafe.
  bool everFed() const { return fed_; }

 private:
  Millis timeout_ms_;
  Millis last_feed_ms_ = 0;
  bool fed_ = false;
};

}  // namespace bzeed

#endif  // BZEED_RELIABILITY_H
