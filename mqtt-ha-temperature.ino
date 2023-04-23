#include "credentials.h"
#include "mqtt.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 // DS18B20 on arduino pin2 corresponds to D4 on physical board "D4 pin on the ndoemcu Module"
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temperature = 19;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
    Serial.begin(115200);
    WiFi.begin(STASSID,STAPSK);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
    DS18B20.begin();
    client.setServer(MQTT_SERVER, MQTT_PORT);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_NAME)) {
      Serial.println("connected");
      client.subscribe(MQTT_COMMAND_TOPIC);
      sendMQTTDiscoveryMsg();
      updateStatus();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendMQTTDiscoveryMsg() {
  DynamicJsonDocument doc(1024);
  char buffer[256];
  doc["name"] = MQTT_NAME;
  doc["command_topic"] = MQTT_COMMAND_TOPIC;
  doc["state_topic"] = MQTT_STATE_TOPIC;
  doc["pl_on"]    = MQTT_COMMAND_ON;
  doc["pl_off"]   = MQTT_COMMAND_OFF;
  doc["stat_t"]   = MQTT_STATE_TOPIC;
  size_t n = serializeJson(doc, buffer);
  client.publish(MQTT_DISCOVERY_TOPIC, buffer, n);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  delay(1000);
  updateStatus();
  Serial.println(temperature);
}

void updateStatus() {
  DS18B20.requestTemperatures(); 
  temperature = DS18B20.getTempCByIndex(0); // Celsius
  char cstr[16];
  itoa(temperature, cstr, 10);
  client.publish(MQTT_STATE_TOPIC, cstr);
  temperature++;
}
