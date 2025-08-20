#include <Arduino.h>
#include <NimBLEDevice.h>
#include "driver/twai.h"
#include <limits.h>

// ---- CAN (TWAI) setup ----
static const twai_timing_config_t CAN_TIMING = TWAI_TIMING_CONFIG_500KBITS();
static const twai_filter_config_t CAN_FILTER = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static twai_general_config_t CAN_CONFIG = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)21 /* TX */, (gpio_num_t)22 /* RX */, TWAI_MODE_NORMAL);

// ---- BLE NUS UUIDs ----
static const char* const NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* const NUS_RX_UUID      = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // phone->ESP32 (Write)
static const char* const NUS_TX_UUID      = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // ESP32->phone (Notify)

static NimBLEServer* bleServer = nullptr;
static NimBLECharacteristic* txChar = nullptr;
static volatile bool deviceConnected = false;

struct Readings { int rpm = -1; int speedKph = -1; int coolantC = INT16_MIN; };

static bool canRequestPid(uint8_t pid) {
  twai_message_t msg = {0};
  msg.identifier = 0x7DF; // functional OBD request
  msg.extd = 0; msg.rtr = 0; msg.ss = 0;
  msg.data_length_code = 8;
  msg.data[0] = 0x02;     // number of additional data bytes
  msg.data[1] = 0x01;     // mode 01 (current data)
  msg.data[2] = pid;      // PID
  msg.data[3] = 0x55; msg.data[4] = 0x55; msg.data[5] = 0x55; msg.data[6] = 0x55; msg.data[7] = 0x55;
  return twai_transmit(&msg, pdMS_TO_TICKS(20)) == ESP_OK;
}

static bool canReadReply(uint8_t pid, uint8_t* A, uint8_t* B) {
  twai_message_t rx;
  uint32_t deadline = millis() + 30; // ~30 ms window
  while (millis() < deadline) {
    if (twai_receive(&rx, pdMS_TO_TICKS(5)) == ESP_OK) {
      if (rx.rtr || rx.data_length_code < 5) continue;
      // Expect: len, 0x41, PID, A, B, ...
      if (rx.data[1] == 0x41 && rx.data[2] == pid) {
        *A = rx.data[3];
        *B = rx.data[4];
        return true;
      }
    }
  }
  return false;
}

static Readings readPidsOnce() {
  Readings r;
  uint8_t A = 0, B = 0;
  // RPM (0x0C) = ((A*256)+B)/4
  if (canRequestPid(0x0C) && canReadReply(0x0C, &A, &B)) r.rpm = ((uint16_t(A) << 8) | B) / 4;
  // Speed (0x0D) = A
  if (canRequestPid(0x0D) && canReadReply(0x0D, &A, &B)) r.speedKph = A;
  // Coolant (0x05) = A - 40
  if (canRequestPid(0x05) && canReadReply(0x05, &A, &B)) r.coolantC = int(A) - 40;
  return r;
}

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override {
    (void)server; (void)connInfo;
    deviceConnected = true;
  }
  void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override {
    (void)connInfo; (void)reason;
    deviceConnected = false;
    server->startAdvertising();
  }
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override {
    (void)characteristic; (void)connInfo;
    // Optional: parse commands to change PID set or period
  }
};

static void initCAN() {
  ESP_ERROR_CHECK(twai_driver_install(&CAN_CONFIG, &CAN_TIMING, &CAN_FILTER));
  ESP_ERROR_CHECK(twai_start());
}

static void initBLE() {
  NimBLEDevice::init("ESP32-OBD");
  bleServer = NimBLEDevice::createServer();
  bleServer->setCallbacks(new ServerCallbacks());
  NimBLEService* svc = bleServer->createService(NUS_SERVICE_UUID);
  NimBLECharacteristic* rx = svc->createCharacteristic(NUS_RX_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  rx->setCallbacks(new RxCallbacks());
  txChar = svc->createCharacteristic(NUS_TX_UUID, NIMBLE_PROPERTY::NOTIFY);
  svc->start();
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(svc->getUUID());
  adv->enableScanResponse(true);
  adv->setName("ESP32-OBD");
  bleServer->startAdvertising();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Booting ESP32 OBD BLE...");
  initCAN();
  initBLE();
  Serial.println("Ready.");
}

void loop() {
  static uint32_t last = 0;
  const uint32_t periodMs = 500;
  if (millis() - last >= periodMs) {
    last = millis();
    Readings r = readPidsOnce();

    if (deviceConnected && txChar != nullptr) {
      char buf[128];
      // Coolant may be INT16_MIN if never read; print as null
      if (r.coolantC == INT16_MIN) {
        snprintf(buf, sizeof(buf), "{\"t\":\"telemetry\",\"rpm\":%d,\"spd_kph\":%d}\n", r.rpm, r.speedKph);
      } else {
        snprintf(buf, sizeof(buf), "{\"t\":\"telemetry\",\"rpm\":%d,\"spd_kph\":%d,\"coolant_c\":%d}\n", r.rpm, r.speedKph, r.coolantC);
      }
      txChar->setValue((uint8_t*)buf, strlen(buf));
      txChar->notify();
    }
  }
}
