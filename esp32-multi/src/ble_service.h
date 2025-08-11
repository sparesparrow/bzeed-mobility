#pragma once

#include <Arduino.h>

class BleServiceWrapper {
public:
  void begin();
  void notifyStatus(const String& status);
};