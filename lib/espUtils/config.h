#ifndef _GLOBAL_VARS
#define _GLOBAL_VARS

#define _DEBUG_ config.debug

#include <getCfg.h>

// extern GlobalConfig config;

extern espCFG mycfg;

static void cfgBegin() {
    mycfg.loadConfiguration();  
    return;
};

// These are the configirable parameters for the ESP
   
    #ifndef STASSID        
        #define esp_mode WIFI_STA     //WIFI_STA=1, WIFI_AP=2 or WIFI_AP_STA=3           
        #define DHTTYPE DHT11      
        // const int DHTTYPE = 11;  
        static const int DHTPIN = 2;
        #define conRetry config.wirelesscfg.connRetries
        static int wsport = 90;
    #endif

    #ifndef LOADMOD
        #define doOTA config.sensorscfg.OTA
        #define doOTApull true
        #define snsTemp config.sensorscfg.Temperature
        #define snsHumid config.sensorscfg.Humidity
        #define snsLux  config.sensorscfg.Light
        #define snsSoilMoist config.sensorscfg.Soil_moisture
        #define snsSoilSlt config.sensorscfg.Soil_salt
        #define snsBatt config.sensorscfg.Battery
        #define snsLvl config.sensorscfg.Level
        #define snsWater config.sensorscfg.Water
        #define doMQTT config.sensorscfg.MQTT
        #define doWebSvr config.sensorscfg.WebSvr
    #endif //LOADMOD

    // static const char* ssid = config.wirelesscfg.ssid;
    // static const char* password = config.wirelesscfg.psk;
    // static const char* host_name = config.wirelesscfg.hostname;
    // static const int conRetries = config.wirelesscfg.connRetries;
    
    static int TIME_TO_WAIT = 2000;

// *******************************************************************************************************************************
// END userdefined data
// *******************************************************************************************************************************


#endif