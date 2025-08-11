#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>

class HttpsClientWrapper {
public:
  bool simpleGet(const char* host, uint16_t port, const char* path, String& responseBody);
};