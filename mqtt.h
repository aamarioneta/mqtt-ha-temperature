#define MQTT_SERVER "192.168.178.200"
#define MQTT_PORT 1883
#define MQTT_NAME "temp2"
#define MQTT_DISCOVERY_TOPIC "homeassistant/sensor/" MQTT_NAME "/config"
#define MQTT_STATE_TOPIC "house/" MQTT_NAME "/state"
#define MQTT_COMMAND_TOPIC "homeassistant/switch/" MQTT_NAME "/set"
#define MQTT_COMMAND_ON "ON"
#define MQTT_COMMAND_OFF "OFF"
