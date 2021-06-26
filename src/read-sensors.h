#pragma once

#ifndef __espReadSensor_H
#define __espReadSensor_H

#include <FS.h>

class espReadSensor {
    public:
        espReadSensor();
        ~espReadSensor();            
        static void setupReadSensor();
        static void getReadSensorCfg();
        static uint32_t readSalt();
        static uint16_t readSoil();
        static float readBattery();
    private:

};      
#endif /* __espReadSensor_H */