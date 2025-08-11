#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class WifiManagerWrapper;

class MqttClientWrapper {
public:
  void begin(WifiManagerWrapper* wifiManager);
  void loop();
  bool ensureConnected();
  bool publish(const String& topic, const String& payload, bool retained = false);

private:
  void handleMessage(char* topic, uint8_t* payload, unsigned int length);
  String buildClientId();

  WifiManagerWrapper* wifi = nullptr;
  WiFiClient netClient;
  PubSubClient client{netClient};
  unsigned long lastConnAttemptMs = 0;
};