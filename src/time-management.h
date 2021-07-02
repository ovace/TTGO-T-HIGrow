#pragma once

#ifndef __timeMgmt_H
#define __timeMgmt_H


class timeMgmt {
    public:
        timeMgmt();
        ~timeMgmt();            
        static boolean setupespTimeMgmt();
        static void getTimeMgmtCfg();
        // static void getTime();
        static unsigned long getEpochTime();
        static String getFormattedTime();
        static void getDate();
        static void getTimeStamp();
        static void showTime();
    private:
        boolean UpdateLocalTime(String Format); 
        void getNTP(const long utcOffsetInSeconds);

};      
#endif /* _timeMgmt_H */

