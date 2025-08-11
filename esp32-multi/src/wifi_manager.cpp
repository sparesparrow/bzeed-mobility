#include "wifi_manager.h"
#include <WiFi.h>
#include "config_wifi.h"

void WifiManagerWrapper::begin() {
  uint64_t chipId = ESP.getEfuseMac();
  char host[32];
  snprintf(host, sizeof(host), "%s-%04llX", WIFI_HOSTNAME_PREFIX, (chipId & 0xFFFF));
  deviceHostname = host;

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(deviceHostname.c_str());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

bool WifiManagerWrapper::ensureConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  unsigned long now = millis();
  if (now - lastAttemptMs < 2000) {
    return false;
  }
  lastAttemptMs = now;

  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(100);
    yield();
  }
  return WiFi.status() == WL_CONNECTED;
}

IPAddress WifiManagerWrapper::localIp() const {
  return WiFi.localIP();
}

int32_t WifiManagerWrapper::rssi() const {
  return WiFi.RSSI();
}