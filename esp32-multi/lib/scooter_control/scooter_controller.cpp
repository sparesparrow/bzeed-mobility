// scooter_controller.cpp — see scooter_controller.h for the design.

#include "scooter_controller.h"

namespace bzeed {
namespace scooter {

ScooterController::ScooterController(const ControllerConfig& cfg)
    : cfg_(cfg),
      keepalive_(cfg.keepalive_period_ms),
      link_(cfg.link_timeout_ms),
      backoff_(cfg.backoff_base_ms, cfg.backoff_max_ms) {}

bool ScooterController::addTransport(ITransport* transport) {
  if (transport == nullptr || transport_count_ >= kMaxTransports) {
    return false;
  }
  transports_[transport_count_++] = transport;
  return true;
}

void ScooterController::requestRun(Millis now_ms) {
  // A run request is also proof of a live controller, so feed the watchdog.
  link_.feed(now_ms);
  if (state_ != State::kRunning) {
    state_ = State::kRunning;
    keepalive_.reset(now_ms);  // send the first frame promptly
    backoff_.reset();
  }
}

void ScooterController::requestStop(Millis now_ms) {
  link_.feed(now_ms);
  if (state_ != State::kLocked) {
    state_ = State::kLocked;
    keepalive_.reset(now_ms);
    backoff_.reset();
  }
}

void ScooterController::heartbeat(Millis now_ms) { link_.feed(now_ms); }

void ScooterController::tick(Millis now_ms) {
  // 1) Supervise the control link. Losing it while running is the dangerous
  //    case: force a failsafe stop rather than let the ESC coast on the last
  //    authorised speed until its own internal timeout.
  if (state_ == State::kRunning && link_.expired(now_ms)) {
    state_ = State::kFailsafe;
    keepalive_.reset(now_ms);
    backoff_.reset();
  }

  // 2) In failsafe, once the link is confirmed silent we keep emitting stop
  //    frames. If a fresh heartbeat/request arrives it will move us out of
  //    failsafe via requestRun()/requestStop().
  if (state_ == State::kFailsafe && !link_.expired(now_ms)) {
    // Link recovered but no explicit intent yet: settle into locked/stop.
    state_ = State::kLocked;
    keepalive_.reset(now_ms);
  }

  // 3) Emit a frame when the keep-alive cadence is due, or keep retrying with
  //    backoff if the last delivery failed.
  const bool cadence_due = keepalive_.due(now_ms);
  const bool retry_due = !last_send_ok_ && backoff_.ready(now_ms);
  if (cadence_due || retry_due) {
    emitCurrentFrame(now_ms);
  }
}

void ScooterController::emitCurrentFrame(Millis now_ms) {
  Command cmd;
  std::uint8_t speed = 0;
  switch (state_) {
    case State::kRunning:
      cmd = Command::run(cfg_.light_on, cfg_.fast_accel);
      speed = cfg_.speed_kph;
      break;
    case State::kLocked:
    case State::kFailsafe:
      cmd = Command::stop(/*light_blink=*/state_ == State::kFailsafe);
      speed = 0;
      break;
  }

  std::uint8_t frame[kFrameSize];
  const std::size_t len = encodeFrame(cmd, speed, frame);
  deliver(frame, len, now_ms);
}

bool ScooterController::deliver(const std::uint8_t* frame, std::size_t len,
                               Millis now_ms) {
  // Redundant delivery: try transports in preference order, preferring
  // healthy ones first, and fall back to the rest if those fail.
  last_send_ok_ = false;
  last_transport_used_ = -1;

  for (int pass = 0; pass < 2 && !last_send_ok_; ++pass) {
    const bool want_healthy = (pass == 0);
    for (std::size_t i = 0; i < transport_count_; ++i) {
      ITransport* t = transports_[i];
      if (t == nullptr) continue;
      if (want_healthy && !t->healthy()) continue;
      if (t->send(frame, len)) {
        last_send_ok_ = true;
        last_transport_used_ = static_cast<int>(i);
        ++frames_sent_;
        break;
      }
    }
  }

  if (last_send_ok_) {
    backoff_.reset();
  } else {
    ++send_failures_;
    backoff_.recordAttempt(now_ms);
  }
  return last_send_ok_;
}

}  // namespace scooter
}  // namespace bzeed
