#include "mqtt_client.h"
#include "wifi_manager.h"
#include "config_mqtt.h"
#include "secrets.h"

void MqttClientWrapper::begin(WifiManagerWrapper* wifiManager) {
  wifi = wifiManager;
  client.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  client.setCallback([this](char* topic, uint8_t* payload, unsigned int length){
    this->handleMessage(topic, payload, length);
  });
}

void MqttClientWrapper::loop() {
  client.loop();
}

bool MqttClientWrapper::ensureConnected() {
  if (client.connected()) {
    return true;
  }
  if (!wifi || !wifi->ensureConnected()) {
    return false;
  }
  unsigned long now = millis();
  if (now - lastConnAttemptMs < 3000) {
    return false;
  }
  lastConnAttemptMs = now;

  String clientId = buildClientId();
  bool ok = false;
  if (strlen(MQTT_USERNAME) > 0) {
    ok = client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
  } else {
    ok = client.connect(clientId.c_str());
  }
  if (ok) {
    client.subscribe(MQTT_COMMAND_TOPIC);
  }
  return ok;
}

bool MqttClientWrapper::publish(const String& topic, const String& payload, bool retained) {
  if (!ensureConnected()) return false;
  return client.publish(topic.c_str(), payload.c_str(), retained);
}

void MqttClientWrapper::handleMessage(char* topic, uint8_t* payload, unsigned int length) {
  // Placeholder for command handling
}

String MqttClientWrapper::buildClientId() {
  uint64_t chipId = ESP.getEfuseMac();
  char id[48];
  snprintf(id, sizeof(id), MQTT_CLIENT_ID_PREFIX "%04llX", (chipId & 0xFFFF));
  return String(id);
}