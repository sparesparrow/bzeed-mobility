#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("ESP32 booting...");
}

void loop() {
  Serial.println("Hello from ESP32");
  delay(1000);
}