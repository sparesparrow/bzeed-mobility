#include "ble_service.h"
#include <NimBLEDevice.h>
#include "config_ble.h"

static NimBLEServer* g_server = nullptr;
static NimBLECharacteristic* g_statusChar = nullptr;

void BleServiceWrapper::begin() {
  NimBLEDevice::init(BLE_DEVICE_NAME);
  g_server = NimBLEDevice::createServer();

  NimBLEService* service = g_server->createService(BLE_SERVICE_UUID);
  g_statusChar = service->createCharacteristic(
    BLE_CHAR_STATUS_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  g_statusChar->setValue("ready");

  service->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(service->getUUID());
  adv->setScanResponse(true);
  NimBLEDevice::startAdvertising();
}

void BleServiceWrapper::notifyStatus(const String& status) {
  if (g_statusChar) {
    g_statusChar->setValue(status.c_str());
    g_statusChar->notify();
  }
}