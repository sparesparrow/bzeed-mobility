// scooter_controller.h
//
// Reliability + redundancy layer on top of the ES200-B protocol codec.
//
// Responsibilities:
//   * Keep-alive: the ESC re-locks itself if it stops hearing valid "run"
//     frames, so a running scooter must be refreshed at a fixed cadence.
//   * Link supervision: a watchdog fed by app/heartbeat traffic. If the
//     controlling link goes silent while running, the scooter is driven to a
//     failsafe (stop) state instead of coasting on stale authorisation.
//   * Redundant transports: up to two output links (e.g. primary UART to the
//     ESC plus a secondary/relayed path). A frame is delivered over the first
//     healthy transport; if it fails, the next is tried, so a single link
//     fault does not drop control.
//   * Retry with backoff when every transport is failing.
//
// The class performs no I/O and reads no clock of its own: callers drive it
// with tick(now_ms) and inject transports. That keeps the whole state machine
// unit-testable on a host with fakes.

#ifndef BZEED_SCOOTER_CONTROLLER_H
#define BZEED_SCOOTER_CONTROLLER_H

#include <cstddef>
#include <cstdint>

#include "reliability.h"
#include "scooter_protocol.h"

namespace bzeed {
namespace scooter {

// Output link abstraction. Implementations wrap a real UART, a BLE
// characteristic, an MQTT-relayed command channel, etc. Kept tiny so a test
// fake is trivial.
class ITransport {
 public:
  virtual ~ITransport() = default;

  // Attempt to send `len` bytes. Return true only if the bytes were handed
  // off successfully. Must not block.
  virtual bool send(const std::uint8_t* data, std::size_t len) = 0;

  // Whether this transport currently believes it can deliver (link up).
  virtual bool healthy() const = 0;

  // Short label for logging/diagnostics.
  virtual const char* name() const = 0;
};

// High-level scooter state exposed for telemetry/UI.
enum class State : std::uint8_t {
  kLocked,    // powered down / not authorised
  kRunning,   // authorised, keep-alive frames being sent
  kFailsafe,  // link lost while running: forcing stop
};

// Tunable timing. Defaults are conservative and match the community-observed
// behaviour of the ES200-B ESC.
struct ControllerConfig {
  Millis keepalive_period_ms = 250;   // cadence of run/stop refresh frames
  Millis link_timeout_ms = 2000;      // silence before failsafe trips
  Millis backoff_base_ms = 100;       // retry delay when all transports fail
  Millis backoff_max_ms = 2000;
  std::uint8_t speed_kph = kDefaultSpeedKph;
  bool light_on = true;
  bool fast_accel = false;
};

class ScooterController {
 public:
  static constexpr std::size_t kMaxTransports = 2;

  explicit ScooterController(const ControllerConfig& cfg = ControllerConfig{});

  // Register an output link. Order = preference (index 0 is primary). Extra
  // transports beyond kMaxTransports are ignored. Returns false if full.
  bool addTransport(ITransport* transport);

  // Operator/app intent. These change the target state; frames are actually
  // emitted from tick().
  void requestRun(Millis now_ms);
  void requestStop(Millis now_ms);

  // Feed the link watchdog. Call whenever a valid control/heartbeat message is
  // received from the authorised controller.
  void heartbeat(Millis now_ms);

  // Drive the state machine. Call every loop iteration. Emits keep-alive
  // frames on schedule, trips the failsafe on link loss, and retries with
  // backoff when transports fail.
  void tick(Millis now_ms);

  // --- diagnostics (const) ---
  State state() const { return state_; }
  bool lastSendOk() const { return last_send_ok_; }
  // Which transport index delivered the last successful frame, or -1.
  int lastTransportUsed() const { return last_transport_used_; }
  std::uint32_t framesSent() const { return frames_sent_; }
  std::uint32_t sendFailures() const { return send_failures_; }

 private:
  // Build the frame for the current target and try to deliver it redundantly.
  void emitCurrentFrame(Millis now_ms);
  bool deliver(const std::uint8_t* frame, std::size_t len, Millis now_ms);

  ControllerConfig cfg_;
  ITransport* transports_[kMaxTransports] = {nullptr, nullptr};
  std::size_t transport_count_ = 0;

  State state_ = State::kLocked;
  Interval keepalive_;
  Watchdog link_;
  Backoff backoff_;

  bool last_send_ok_ = false;
  int last_transport_used_ = -1;
  std::uint32_t frames_sent_ = 0;
  std::uint32_t send_failures_ = 0;
};

}  // namespace scooter
}  // namespace bzeed

#endif  // BZEED_SCOOTER_CONTROLLER_H
