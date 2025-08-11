#include "status_publisher.h"
#include "wifi_manager.h"
#include "gps_manager.h"
#include <ArduinoJson.h>

void StatusPublisher::begin(WifiManagerWrapper* wifiMgr, GpsManagerWrapper* gpsMgr) {
  wifi = wifiMgr;
  gps = gpsMgr;
  bootMs = millis();
}

String StatusPublisher::buildStatusJson() {
  StaticJsonDocument<512> doc;
  doc["uptime_ms"] = millis() - bootMs;

  if (wifi) {
    doc["wifi"]["rssi"] = wifi->rssi();
    doc["wifi"]["ip"] = wifi->localIp().toString();
    doc["wifi"]["hostname"] = wifi->hostname();
  }

  if (gps && gps->hasFix()) {
    doc["gps"]["lat"] = gps->latitude();
    doc["gps"]["lon"] = gps->longitude();
    doc["gps"]["hdop"] = gps->hdop();
    doc["gps"]["sats"] = gps->satellites();
  }

  String out;
  serializeJson(doc, out);
  return out;
}