#include "save-configuration.h"
#include "user-variables.h"

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <PubSubClient.h>

#include "getCfg.h"
#include "config.h"
#include "connect-to-network.h"
#include "file-management.h"
#include "go-to-deep-sleep.h"

// mqtt constants
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// Allocate a  JsonDocument
void saveConfiguration(const SensorData & sensordata) {

  //  Serial.println(WiFi.macAddress());
  //  String stringMAC = WiFi.macAddress();
  //  stringMAC.replace(':', '_');

  byte mac[6];
  WiFi.macAddress(mac);

  //  String chipId = String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
  String chipId = "";
  String HEXcheck = "";
  for (int i = 0; i <= 5; i++) {
    HEXcheck = String(mac[i], HEX);
    if (HEXcheck.length() == 1) {
      chipId = chipId + "0" + String(mac[i], HEX);
    } else {
      chipId = chipId + String(mac[i], HEX);
    }
  }
  Serial.println("chipId " + chipId);
  const String topicStr = "esp/" + config.devicecfg.name + "/" + chipId;
  const char* topic = topicStr.c_str();
  if (_DEBUG_) {
    Serial.println(topic);
    Serial.println(ssid);
  };

  StaticJsonDocument<1024> doc;
  // Set the values in the document
  // Device changes according to device placement
  JsonObject root = doc.to<JsonObject>();  
  JsonObject garden = root.createNestedObject("device_placement");
  garden["device_placement"] = config.devicecfg.location;
  JsonObject deviceStats = doc.createNestedObject("deviceStats");
  deviceStats[config.devicecfg.model] = chipId;
  deviceStats["sensorname"] = config.devicecfg.name;
  deviceStats["date"] = sensordata.date;
  deviceStats["time"] = sensordata.time;
  deviceStats["TZ"] = sensordata.TZ;
  deviceStats["DST"] = sensordata.DST;
  deviceStats["upTime"] = sensordata.uptime;
  deviceStats["sleep5Count"] = sensordata.sleep5no;
  deviceStats["bootCount"] = sensordata.bootno;
  JsonObject sensorReads = doc.createNestedObject("sensorReads");
  sensorReads["lux"] = sensordata.lux;
  sensorReads["temp"] = sensordata.temp;
  sensorReads["humid"] = sensordata.humid;
  sensorReads["soil"] = sensordata.soil;
  sensorReads["salt"] = sensordata.salt;
  sensorReads["saltadvice"] = sensordata.saltadvice;
  sensorReads["bat"] = sensordata.bat;
  sensorReads["batcharge"] = sensordata.batcharge;
  sensorReads["battvolt"] = sensordata.batvolt;
  sensorReads["battvoltage"] = sensordata.batvoltage;
  sensorReads["wifissid"] = WiFi.SSID();
  JsonObject codeStats = doc.createNestedObject("codeStats");
  codeStats["fw_rel"] = sensordata.rel;
  codeStats["conf_rel"] = config.cfg_rel;

  // Send to mqtt
  char buffer[1024];
  serializeJson(doc, buffer);


  Serial.print("Sending message to topic: ");
  if (logging) {
    writeFile(SPIFFS, "/error.log", "Sending message to topic: \n");
  }

  Serial.println(buffer);

  // Connect to mqtt broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  if (logging) {
    writeFile(SPIFFS, "/error.log", "Attempting to connect to the MQTT broker! \n");
  }

  Serial.println(broker);
  mqttClient.setServer(broker, 1883);

  if (!mqttClient.connect(broker, mqttuser, mqttpass)) {
    if (logging) {
      writeFile(SPIFFS, "/error.log", "MQTT connection failed! \n");
    }

    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.state());
    goToDeepSleepFiveMinutes();
  }

  if (logging) {
    writeFile(SPIFFS, "/error.log", "You're connected to the MQTT broker! \n");
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  bool retained = true;

  if (mqttClient.publish(topic, buffer, retained)) {
    Serial.println("Message published successfully");
  } else {
    Serial.println("Error in Message, not published");
    goToDeepSleepFiveMinutes();
  }
  Serial.println();
}

