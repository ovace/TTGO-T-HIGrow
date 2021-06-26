// Start Subroutines
#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <ESP.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>  
  #include <WiFiClient.h>
  #include <ESP8266mDNS.h>
  #include <LITTLEFS.h>
#elif defined(ESP32)
  #include <WiFi.h>  
  // Logfile on SPIFFS
  #include "SPIFFS.h"
#else
  #error Invalid platform
#endif 

#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>

#include "read-sensors.h"
extern espReadSensor readsensor;
#include <config.h>
#include <getCfg.h>
espCFG mycfg;
#include <go-to-deep-sleep.h>
extern go2Sleep go2sleep;
#include <get-string-value.h>
#include <save-configuration.h>
extern saveCfg savecfg;
#include <connect-to-network.h>
extern espNWconn espnwconn;
#include <file-management.h>
extern espFileMgmt espfilemgmt;
#include <time-management.h>
extern timeMgmt timemgmt;


static String code_rel = "0.01 "; // ESP sensor project

// Reboot counters
static RTC_DATA_ATTR int bootCount = 0;  
static RTC_DATA_ATTR int sleep5no = 0;
static const int led = 13;

#define I2C_SDA             25
#define I2C_SCL             26
// #define DHT_PIN             16
#define BOOT_PIN            0
#define POWER_CTRL          4
#define USER_BUTTON         35

BH1750 lightMeter(0x23); //0x23
Adafruit_BME280 bmp;     //0x77 Adafruit_BME280 is technically not used, but if removed the BH1750 will not work - Any suggestions why, would be appriciated.

// DHT dht(config.dhtcfg.pin, config.dhtcfg.type);
DHT dht(DHTPIN, DHTTYPE);

static bool bme_found = false;

void setup() {
  Serial.println("Void Setup");  
  
  mycfg.loadConfiguration();  

  int _espbaud = config.serialcomcfg.espbaud;
  if(_espbaud <= 0){ _espbaud = 115200;};

  Serial.begin(_espbaud);

  if ( _DEBUG_ ) {
    Serial.setDebugOutput(true);
    Serial.println();

    Serial.println(F("Main Debug info"));
    mycfg.printFile(); 
    mycfg.printCFG();  

    Serial.printf("retry counter: %d\n", config.wirelesscfg.connRetries);

    Serial.printf("Enable OTA: %s\n", doOTA?"Yes":"No");
    Serial.printf("MQTT: %s\n", doMQTT?"Yes":"No");
    Serial.printf("WebSvr: %s\n", doWebSvr?"Yes":"No");
    Serial.printf("Temperature: %s\n", snsTemp?"Yes":"No");
    Serial.printf("Humidity: %s\n", snsHumid?"Yes":"No");
    Serial.printf("Light: %s\n", snsLux?"Yes":"No");
    Serial.printf("Soil_Moisture: %s\n", snsSoilMoist?"Yes":"No");
    Serial.printf("Soil_salt: %s\n", snsSoilSlt?"Yes":"No");
    Serial.printf("Battery: %s\n", snsBatt?"Yes":"No");
    Serial.printf("Level: %s\n", snsLvl?"Yes":"No");
    Serial.printf("Water: %s\n", snsWater?"Yes":"No");
  }

  // Start WiFi and update time
  espnwconn.connectToNetwork();
  Serial.println(" ");
  Serial.println("Connected to network");
 
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Connected to network \n");
  } 
 

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  // configTime(gmtOffset_sec, 0, config.ntpcfg.Server.c_str());
  //  timeClient.setTimeOffset(7200);
  timemgmt.setupespTimeMgmt();
  timemgmt.showTime();
  timemgmt.getDate();
  timemgmt.getTime();

  Wire.begin(I2C_SDA, I2C_SCL);
  
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Wire Begin OK! \n");
  } 
 

  dht.begin();
  
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "DHT12 Begin OK! \n");
  } 
  

  //! Sensor power control pin , use deteced must set high
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 1);
  delay(1000);

  if (!bmp.begin()) {
    Serial.println(F("This check must be done, otherwise the BH1750 does not initiate!!!!?????"));
    bme_found = false;
  } else {
    bme_found = true;
  }

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  float luxRead = lightMeter.readLightLevel();
  Serial.print("lux ");
  Serial.println(luxRead);
  delay(2000);
  float t12 = dht.readTemperature(true); // Read temperature as Centigrade then dht.readTemperature()
  config.sensorcfg.temp = t12;
  Serial.print("temp *F: ");
  Serial.println(t12);
  delay(2000);
  float h12 = dht.readHumidity();
  config.sensorcfg.humid = h12;
  Serial.print("Humid %: ");
  Serial.println(h12);
  uint16_t soil = readsensor.readSoil();
  config.sensorcfg.soil = soil;
  Serial.print("Soil: ");
  Serial.println(soil);
  uint32_t salt = readsensor.readSalt();
  config.sensorcfg.salt = salt;
  Serial.print("Salt: ");
  Serial.println(salt);
  String advice;
  if (salt < 201) {
    advice = "needed";
  }
  else if (salt < 251) {
    advice = "low";
  }
  else if (salt < 351) {
    advice = "optimal";
  }
  else if (salt > 350) {
    advice = "too high";
  }
  Serial.println (advice);
  config.sensorcfg.saltadvice = advice;



  float bat = readsensor.readBattery();
  config.sensorcfg.bat = bat;
  Serial.print("Batt: ");
  Serial.println(bat);
  config.sensorcfg.batcharge = "";
  if (bat > 130) {
    config.sensorcfg.batcharge = "charging";
  }

  if (bat > 100) {
    config.sensorcfg.bat = 100;
  }
  
  config.sensorcfg.bootno = bootCount;
  Serial.print("Boot Count: ");
  Serial.println(bootCount);


  luxRead = lightMeter.readLightLevel();
  Serial.print("lux ");
  Serial.println(luxRead);
  config.sensorcfg.lux = luxRead;
  config.sensorcfg.code_rel = code_rel;

  // Create JSON file
  Serial.println(F("Creating JSON document..."));
 
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Creating JSON document...! \n");
  } 
  
  savecfg.saveConfiguration(config);

  // Go to sleep
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Go to sleep now
  delay(1000);
  go2sleep.goToDeepSleep(config.timerscfg.TIME_TO_SLEEP);
  
}

void loop() {
}
