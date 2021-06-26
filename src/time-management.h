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
        static unsigned long getTime();
        static void getDate();
        static void getTimeStamp();
        static void showTime();
    private:
        static boolean UpdateLocalTime(String Format); 

};      
#endif /* _timeMgmt_H */

