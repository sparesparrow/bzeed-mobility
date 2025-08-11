#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_client.h"
#include "ble_service.h"
#include "gps_manager.h"
#include "status_publisher.h"
#include "config_mqtt.h"

WifiManagerWrapper wifiManager;
MqttClientWrapper mqttClient;
BleServiceWrapper bleService;
GpsManagerWrapper gpsManager;
StatusPublisher statusPublisher;

unsigned long lastStatusPublishMs = 0;
const unsigned long STATUS_PUBLISH_INTERVAL_MS = 30000; // 30 seconds

void setup() {
  Serial.begin(115200);
  delay(200);

  wifiManager.begin();
  mqttClient.begin(&wifiManager);
  bleService.begin();
  gpsManager.begin();
  statusPublisher.begin(&wifiManager, &gpsManager);
}

void loop() {
  // Maintain services
  wifiManager.ensureConnected();
  mqttClient.ensureConnected();
  mqttClient.loop();
  gpsManager.loop();

  // Periodic status publish
  unsigned long now = millis();
  if (now - lastStatusPublishMs >= STATUS_PUBLISH_INTERVAL_MS) {
    lastStatusPublishMs = now;
    String statusJson = statusPublisher.buildStatusJson();
    mqttClient.publish(MQTT_STATUS_TOPIC, statusJson, false);
  }

  delay(10);
}