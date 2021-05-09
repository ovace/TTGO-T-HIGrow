#ifndef _DHT_MQTT_webserver_H
#define _DHT_MQTT_webserver_H

#include "espWebSvr.h"

    #include <AsyncMqttClient.h>
    #include <DHT.h>
    #include "config.h"

    class espWebSrv {
        public:
            espWebSrv();
            //~espWebSrv();            
            static void setupWebSvr(const int port);            
            static void WSStatus();   
            static void handleClient();
            static int getWSPort();
            

        private:
            static void initWebSvr(const int port) ;              
            static void restRouter();            
            static void handleRoot(); 
            static void handleNotFound();  
    };    

    class espSwitch {
        public:
            espSwitch();
            //~espSwitchv();            
            static void handleSwitchOFF();
            static void handleSwitchON();   
            

        private:
            static bool relayClose();
            static bool relayOpen();    
            
    };  
  
#endif