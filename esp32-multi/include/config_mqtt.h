#pragma once

#include "secrets.h"

#define MQTT_CLIENT_ID_PREFIX "esp32-multi-"
#define MQTT_BASE_TOPIC      "devices/esp32multi"
#define MQTT_STATUS_TOPIC    MQTT_BASE_TOPIC "/status"
#define MQTT_COMMAND_TOPIC   MQTT_BASE_TOPIC "/cmd"
#define MQTT_QOS 0