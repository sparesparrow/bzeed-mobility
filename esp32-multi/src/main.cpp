#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_client.h"
#include "ble_service.h"
#include "gps_manager.h"
#include "status_publisher.h"
#include "config_mqtt.h"
#include "scooter_controller.h"
#include "scooter_uart_transport.h"

using bzeed::scooter::ScooterController;
using bzeed::scooter::ControllerConfig;
using bzeed::scooter::State;

WifiManagerWrapper wifiManager;
MqttClientWrapper mqttClient;
BleServiceWrapper bleService;
GpsManagerWrapper gpsManager;
StatusPublisher statusPublisher;

// --- Scooter control (ES200-B ESC) ---
// ESC UART on Serial2; adjust pins to your wiring. An optional enable pin can
// gate a level-shifter/relay (pass its GPIO instead of -1).
#define SCOOTER_UART_RX 25
#define SCOOTER_UART_TX 26
#define SCOOTER_ESC_BAUD 9600

ScooterUartTransport escTransport(Serial2, "esc-uart", /*enable_pin=*/-1);
ScooterController scooter{ControllerConfig{}};

unsigned long lastStatusPublishMs = 0;
const unsigned long STATUS_PUBLISH_INTERVAL_MS = 30000; // 30 seconds

// Interpret an inbound command message and drive the controller. Any valid
// command also feeds the link watchdog inside requestRun/requestStop.
static void handleScooterCommand(const String& topic, const String& payload) {
  (void)topic;
  const unsigned long now = millis();
  if (payload.equalsIgnoreCase("RUN") || payload.equalsIgnoreCase("UNLOCK")) {
    scooter.requestRun(now);
  } else if (payload.equalsIgnoreCase("STOP") || payload.equalsIgnoreCase("LOCK")) {
    scooter.requestStop(now);
  } else if (payload.equalsIgnoreCase("PING")) {
    scooter.heartbeat(now);  // keep-alive without changing state
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  wifiManager.begin();
  mqttClient.begin(&wifiManager);
  mqttClient.setCommandHandler(handleScooterCommand);
  bleService.begin();
  gpsManager.begin();
  statusPublisher.begin(&wifiManager, &gpsManager);

  escTransport.begin(SCOOTER_ESC_BAUD, SCOOTER_UART_RX, SCOOTER_UART_TX);
  scooter.addTransport(&escTransport);
}

void loop() {
  const unsigned long now = millis();

  // Maintain services
  wifiManager.ensureConnected();
  mqttClient.ensureConnected();
  mqttClient.loop();
  gpsManager.loop();

  // Drive the scooter state machine every iteration: keep-alive frames, link
  // watchdog and failsafe are all handled inside tick().
  scooter.tick(now);

  // Periodic status publish
  if (now - lastStatusPublishMs >= STATUS_PUBLISH_INTERVAL_MS) {
    lastStatusPublishMs = now;
    String statusJson = statusPublisher.buildStatusJson();
    mqttClient.publish(MQTT_STATUS_TOPIC, statusJson, false);
  }

  delay(5);
}
