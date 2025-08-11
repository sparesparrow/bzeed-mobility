#pragma once

#include <Arduino.h>

class WifiManagerWrapper;
class GpsManagerWrapper;

class StatusPublisher {
public:
  void begin(WifiManagerWrapper* wifi, GpsManagerWrapper* gps);
  String buildStatusJson();

private:
  WifiManagerWrapper* wifi = nullptr;
  GpsManagerWrapper* gps = nullptr;
  unsigned long bootMs = 0;
};