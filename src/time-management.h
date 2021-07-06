#pragma once

#ifndef __timeMgmt_H
#define __timeMgmt_H

struct updateTimeStatus {
    time_t curTime;
    bool status;
};
  
typedef struct updateTimeStatus tmStruct;

class timeMgmt {
    public:
        timeMgmt();
        ~timeMgmt();            
        static void showTime();
        static String getUTCtime(String Format);
        static String getUTCdate(String Format); 
        static String getLocalTzTime(String Format);
        static String getLocalTzDate(String Format);
        static unsigned long getEpochTime();        
        static String getTimeStamp();
        static void showTimeComponents();
    private:
        void getTimeMgmtCfg();   
        void setupNTP();     
        void printTime();   
        tmStruct updateTime(const char* tz);

};      
#endif /* _timeMgmt_H */

