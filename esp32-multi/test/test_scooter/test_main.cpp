// Host-side unit tests for the scooter_control library.
//
// Dependency-free: builds with any C++14 compiler (see run_tests.sh) so it
// runs in CI without hardware, PlatformIO, or a test framework. A non-zero
// exit code fails the build.
//
// The golden CRC/frame vectors below are real frames captured from ES200-B
// scooters by the community projects credited in the library README; they
// pin our clean-room codec to the actual ESC behaviour.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "reliability.h"
#include "scooter_controller.h"
#include "scooter_protocol.h"

using namespace bzeed;
using namespace bzeed::scooter;

static int g_failures = 0;

#define CHECK(cond)                                                      \
  do {                                                                   \
    if (!(cond)) {                                                       \
      std::printf("  FAIL: %s (line %d)\n", #cond, __LINE__);            \
      ++g_failures;                                                      \
    }                                                                    \
  } while (0)

// ---- protocol codec ----

static void test_crc_golden_vectors() {
  std::printf("test_crc_golden_vectors\n");
  // {frame[0..4]} -> expected CRC (frame[5]).
  struct V { std::uint8_t f[5]; std::uint8_t crc; };
  const V vectors[] = {
      {{0xA6, 0x12, 0x02, 0x10, 0x14}, 0xCF},  // stop / power off, 20 km/h field
      {{0xA6, 0x12, 0x02, 0xE5, 0xE4}, 0xDD},  // run + light
      {{0xA6, 0x12, 0x02, 0x05, 0xE4}, 0xA8},  // light on
      {{0xA6, 0x12, 0x02, 0x01, 0xE4}, 0x93},  // light off
      {{0xA6, 0x12, 0x02, 0x03, 0xE4}, 0x02},  // light flash
  };
  for (const auto& v : vectors) {
    CHECK(crc8Maxim(v.f, 5) == v.crc);
  }
}

static void test_command_byte_bits() {
  std::printf("test_command_byte_bits\n");
  // stop() keeps the km/h display bit (bit4) set -> 0x10, matching the real
  // captured "power off" command byte from an ES200-B ESC.
  CHECK(makeCommandByte(Command::stop()) == 0x10);
  // power only -> bit0
  Command power_only; power_only.power_on = true; power_only.units_kph = false;
  CHECK(makeCommandByte(power_only) == 0x01);
  // run() default: power(0) + light(2) + kph(4) = 0x15
  CHECK(makeCommandByte(Command::run()) == 0x15);
  // fast accel adds bit5 (0x20) -> 0x35
  CHECK(makeCommandByte(Command::run(/*light*/ true, /*fast*/ true)) == 0x35);
  // stop with blink adds bit1 -> 0x12
  CHECK(makeCommandByte(Command::stop(/*blink*/ true)) == 0x12);
}

static void test_encode_and_validate() {
  std::printf("test_encode_and_validate\n");
  std::uint8_t frame[kFrameSize];
  CHECK(encodeFrame(Command::run(), 20, frame) == kFrameSize);
  CHECK(frame[0] == kPreamble && frame[1] == kAddress && frame[2] == kPayloadLen);
  CHECK(frame[3] == 0x15 && frame[4] == 20);
  CHECK(frame[5] == crc8Maxim(frame, kCrcSpan));
  CHECK(isValidFrame(frame, kFrameSize));

  // Speed is clamped: never emit above the max.
  encodeFrame(Command::run(), 250, frame);
  CHECK(frame[4] == kMaxSpeedKph);

  // Corruption and bad lengths are rejected.
  frame[4] ^= 0xFF;
  CHECK(!isValidFrame(frame, kFrameSize));
  CHECK(!isValidFrame(frame, kFrameSize - 1));
  CHECK(!isValidFrame(nullptr, kFrameSize));
}

// ---- reliability helpers ----

static void test_backoff_grows_and_resets() {
  std::printf("test_backoff_grows_and_resets\n");
  Backoff b(100, 800);
  CHECK(b.ready(0));           // first attempt allowed immediately
  b.recordAttempt(0);
  CHECK(b.currentDelay() == 200);
  CHECK(!b.ready(199));
  CHECK(b.ready(200));
  b.recordAttempt(200);
  CHECK(b.currentDelay() == 400);
  b.recordAttempt(600);
  b.recordAttempt(1000);
  CHECK(b.currentDelay() == 800);  // capped
  b.reset();
  CHECK(b.currentDelay() == 100);
}

static void test_watchdog_and_interval() {
  std::printf("test_watchdog_and_interval\n");
  Watchdog w(1000);
  CHECK(!w.everFed());
  CHECK(!w.expired(5000));  // never fed -> not expired
  w.feed(100);
  CHECK(!w.expired(500));
  CHECK(w.expired(1100));

  Interval iv(250);
  CHECK(iv.due(0));      // fires immediately
  CHECK(!iv.due(100));
  CHECK(iv.due(250));
  CHECK(!iv.due(400));
  CHECK(iv.due(500));
}

// ---- controller: a fake transport we can steer ----

class FakeTransport : public ITransport {
 public:
  explicit FakeTransport(const char* n) : name_(n) {}
  bool send(const std::uint8_t* data, std::size_t len) override {
    if (!up_ || fail_next_) { fail_next_ = false; return false; }
    last.assign(data, data + len);
    ++sends;
    return true;
  }
  bool healthy() const override { return up_; }
  const char* name() const override { return name_; }

  void setUp(bool up) { up_ = up; }
  void failOnce() { fail_next_ = true; }

  const char* name_;
  bool up_ = true;
  bool fail_next_ = false;
  int sends = 0;
  std::vector<std::uint8_t> last;
};

static void test_controller_run_keepalive() {
  std::printf("test_controller_run_keepalive\n");
  ControllerConfig cfg;  // keepalive 250ms, link timeout 2000ms
  ScooterController c(cfg);
  FakeTransport primary("uart");
  CHECK(c.addTransport(&primary));

  c.requestRun(0);
  c.tick(0);
  CHECK(c.state() == State::kRunning);
  CHECK(primary.sends == 1);
  // Encoded a run frame at configured speed.
  CHECK(primary.last.size() == kFrameSize);
  CHECK(primary.last[3] == makeCommandByte(Command::run()));
  CHECK(primary.last[4] == cfg.speed_kph);

  c.tick(100);
  CHECK(primary.sends == 1);  // not due yet
  c.heartbeat(100);
  c.tick(250);
  CHECK(primary.sends == 2);  // keep-alive fired
}

static void test_controller_failsafe_on_link_loss() {
  std::printf("test_controller_failsafe_on_link_loss\n");
  ControllerConfig cfg;
  ScooterController c(cfg);
  FakeTransport primary("uart");
  c.addTransport(&primary);

  c.requestRun(0);
  c.tick(0);
  CHECK(c.state() == State::kRunning);

  // No heartbeats for longer than link_timeout_ms -> failsafe stop.
  c.tick(2500);
  CHECK(c.state() == State::kFailsafe);
  CHECK(primary.last[3] == makeCommandByte(Command::stop(/*blink*/ true)));
  CHECK(primary.last[4] == 0);  // speed forced to zero

  // A fresh run request recovers control.
  c.requestRun(2600);
  c.tick(2600);
  CHECK(c.state() == State::kRunning);
}

static void test_controller_transport_redundancy() {
  std::printf("test_controller_transport_redundancy\n");
  ScooterController c;
  FakeTransport primary("uart");
  FakeTransport secondary("relay");
  c.addTransport(&primary);
  c.addTransport(&secondary);

  // Primary link down -> frame delivered over secondary.
  primary.setUp(false);
  c.requestRun(0);
  c.tick(0);
  CHECK(c.lastSendOk());
  CHECK(c.lastTransportUsed() == 1);
  CHECK(secondary.sends == 1);
  CHECK(primary.sends == 0);

  // Both down -> failure recorded, backoff engaged.
  secondary.setUp(false);
  c.heartbeat(250);
  c.tick(250);
  CHECK(!c.lastSendOk());
  CHECK(c.sendFailures() >= 1);

  // Primary back up -> recovers, prefers primary (index 0).
  primary.setUp(true);
  c.heartbeat(500);
  c.tick(500);
  CHECK(c.lastSendOk());
  CHECK(c.lastTransportUsed() == 0);
}

int main() {
  std::printf("== scooter_control host tests ==\n");
  test_crc_golden_vectors();
  test_command_byte_bits();
  test_encode_and_validate();
  test_backoff_grows_and_resets();
  test_watchdog_and_interval();
  test_controller_run_keepalive();
  test_controller_failsafe_on_link_loss();
  test_controller_transport_redundancy();

  if (g_failures == 0) {
    std::printf("ALL TESTS PASSED\n");
    return 0;
  }
  std::printf("%d CHECK(s) FAILED\n", g_failures);
  return 1;
}
