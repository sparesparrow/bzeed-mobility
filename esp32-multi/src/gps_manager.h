#pragma once

#include <Arduino.h>
#include <TinyGPSPlus.h>

class GpsManagerWrapper {
public:
  void begin();
  void loop();

  bool hasFix() const { return hasValidFix; }
  double latitude() const { return lastLat; }
  double longitude() const { return lastLon; }
  double hdop() const { return lastHdop; }
  uint32_t satellites() const { return lastSatellites; }

private:
  TinyGPSPlus gps;
  HardwareSerial* gpsSerial = nullptr; // will point to Serial1
  bool hasValidFix = false;
  double lastLat = 0;
  double lastLon = 0;
  double lastHdop = 0;
  uint32_t lastSatellites = 0;
};