/*
  @file save-configuration.cpp


*/
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>  
  #include <WiFiClient.h>
  #include <ESP8266mDNS.h>
  #include <LITTLEFS.h>
#elif defined(ESP32)
  #include <WiFi.h>   
  #include "SPIFFS.h" 
#else
  #error Invalid platform
#endif 

#include "config.h"
// #include "espMQTT-WiFi.h"
// espMQTT espmqtt;

#include "file-management.h"
extern espFileMgmt espfilemgmt;

#include "go-to-deep-sleep.h"  
extern go2Sleep go2sleep;

#include "save-configuration.h"
saveCfg savecfg;

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 


saveCfg::saveCfg() { //Class constructor
};
saveCfg::~saveCfg() { //Class destructor
};

void saveCfg::setupsaveCfg(){  
};
void saveCfg::getsaveCfg(){
};

// Allocate a  JsonDocument
void saveCfg::saveConfiguration(const GlobalConfig  & config) {

  Serial.println(WiFi.macAddress());
  String stringMAC = WiFi.macAddress();
  stringMAC.replace(':', '_');

  static byte mac[6];
  WiFi.macAddress(mac);

  static String chipId = String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
  // static String chipId = "";
  static String HEXcheck = "";
  for (int i = 0; i <= 5; i++) {
    HEXcheck = String(mac[i], HEX);
    if (HEXcheck.length() == 1) {
      chipId = chipId + "0" + String(mac[i], HEX);
    } else {
      chipId = chipId + String(mac[i], HEX);
    }
  }
  Serial.println("chipId " + chipId);

  const String topicStr = config.mqttcfg.home_topic_base + "/" + config.devicecfg.place + "/" + config.devicecfg.name + "/" + config.devicecfg.location + "/" + chipId;
  const char* topic = topicStr.c_str();
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("ssid: ");
  Serial.println(config.wirelesscfg.ssid.c_str());

  StaticJsonDocument<1024> doc;
  // Set the values in the document
  // Device changes according to device placement
  JsonObject root = doc.to<JsonObject>();

  JsonObject plant = root.createNestedObject("plant");
  plant[config.devicecfg.name] = chipId;
  plant["sensorname"] = config.devicecfg.name;
  plant["date"] = config.sensorcfg.date;
  plant["time"] = config.sensorcfg.time;
  plant["tz"] = config.sensorcfg.tz;
  plant["sleep5Count"] = config.sensorcfg.sleep5no;
  plant["bootCount"] = config.sensorcfg.bootno;
  plant["lux"] = config.sensorcfg.lux;
  plant["temp"] = config.sensorcfg.temp;
  plant["humid"] = config.sensorcfg.humid;
  plant["soil"] = config.sensorcfg.soil;
  plant["salt"] = config.sensorcfg.salt;
  plant["saltadvice"] = config.sensorcfg.saltadvice;
  plant["bat"] = config.sensorcfg.bat;
  plant["batcharge"] = config.sensorcfg.batcharge;
  plant["battvolt"] = config.sensorcfg.batvolt;
  plant["battvoltage"] = config.sensorcfg.batvoltage;
  plant["wifissid"] = WiFi.SSID();
  plant["code.rel"] = config.sensorcfg.code_rel;

  // Send to mqtt
  char buffer[1024];
  serializeJson(doc, buffer);


  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Sending message to topic: \n");
  }

  Serial.println(buffer);

  // Connect to mqtt broker

  // // if (doMQTT ) {
  //   Serial.println(F("Enabling MQTT"));
  //   espmqtt.mqttSetup();  
  // // }

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(config.mqttcfg.server);
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Attempting to connect to the MQTT broker! \n");
  }
  
   
  mqttClient.setServer(config.mqttcfg.server.c_str(), config.mqttcfg.port);

  if (!mqttClient.connect(config.mqttcfg.server.c_str(), config.mqttcfg.user.c_str(), config.mqttcfg.pas.c_str())) {
    if (logging) {
      espfilemgmt.writeFile(SPIFFS, "/error.log", "MQTT connection failed! \n");
    }

    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.state());
    go2sleep.goToDeepSleepFiveMinutes();
  }
 
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "You're connected to the MQTT broker! \n");
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  bool retained = true;
  mqttClient.setBufferSize(1024);

  if (mqttClient.publish(topic, buffer, retained)) {
    Serial.println("Message published successfully");
  } else {
    Serial.println("Error in Message, not published");
    go2sleep.goToDeepSleepFiveMinutes();
  }
  Serial.println();

  // // if (doMQTT) {

  //   bool retained = true;
  //   int qos = 0;
  //   Serial.println("Publishing to MQTT");
  //   // Publish an MQTT message on topic esp/dht/temperature
  //   uint16_t mqttPacket = espmqtt.mqttpub(topic, "test message");
  //   // uint16_t mqttPacket = espmqtt.mqttpub(topic, buffer);
  //   Serial.println("mqtt packet: " + mqttPacket);
  // // };

  yield();
}

