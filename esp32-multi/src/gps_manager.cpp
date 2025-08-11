#include "gps_manager.h"
#include "config_gps.h"

void GpsManagerWrapper::begin() {
  gpsSerial = &Serial1;
  gpsSerial->begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void GpsManagerWrapper::loop() {
  if (!gpsSerial) return;
  while (gpsSerial->available()) {
    gps.encode(gpsSerial->read());
  }

  if (gps.location.isUpdated() && gps.location.isValid()) {
    lastLat = gps.location.lat();
    lastLon = gps.location.lng();
    hasValidFix = true;
  }
  if (gps.hdop.isUpdated() && gps.hdop.isValid()) {
    lastHdop = gps.hdop.hdop();
  }
  if (gps.satellites.isUpdated() && gps.satellites.isValid()) {
    lastSatellites = gps.satellites.value();
  }
}