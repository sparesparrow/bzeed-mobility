#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <functional>
#include <utility>

class WifiManagerWrapper;

class MqttClientWrapper {
public:
  // Called for each inbound message on the subscribed command topic.
  using CommandHandler = std::function<void(const String& topic, const String& payload)>;

  void begin(WifiManagerWrapper* wifiManager);
  void loop();
  bool ensureConnected();
  bool publish(const String& topic, const String& payload, bool retained = false);

  // Register a handler for inbound command-topic messages. Decouples command
  // interpretation (e.g. driving the ScooterController) from transport.
  void setCommandHandler(CommandHandler handler) { commandHandler = std::move(handler); }

private:
  void handleMessage(char* topic, uint8_t* payload, unsigned int length);
  String buildClientId();

  WifiManagerWrapper* wifi = nullptr;
  WiFiClient netClient;
  PubSubClient client{netClient};
  unsigned long lastConnAttemptMs = 0;
  CommandHandler commandHandler;
};