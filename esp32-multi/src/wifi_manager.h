#pragma once

#include <Arduino.h>

class WifiManagerWrapper {
public:
  void begin();
  bool ensureConnected();
  IPAddress localIp() const;
  int32_t rssi() const;
  String hostname() const { return deviceHostname; }

private:
  String deviceHostname;
  unsigned long lastAttemptMs = 0;
};