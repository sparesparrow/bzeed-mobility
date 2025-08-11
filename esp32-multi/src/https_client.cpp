#include "https_client.h"
#include "config_https.h"

bool HttpsClientWrapper::simpleGet(const char* host, uint16_t port, const char* path, String& responseBody) {
  WiFiClientSecure client;
  if (strlen(HTTPS_ROOT_CA) > 0) {
    client.setCACert(HTTPS_ROOT_CA);
  } else {
    client.setInsecure();
  }
  if (!client.connect(host, port)) {
    return false;
  }
  client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: esp32-multi\r\nConnection: close\r\n\r\n", path, host);

  unsigned long start = millis();
  while (!client.available()) {
    if (millis() - start > 8000) {
      client.stop();
      return false;
    }
    delay(10);
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    responseBody += line;
  }
  client.stop();
  return true;
}