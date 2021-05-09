  
#ifndef _TIME_MGMT_H
#define _TIME_MGMT_H

#include <Arduino.h>

class timeMGMT {

  public:
    timeMGMT();
    ~timeMGMT();
    void setup();
    bool chkDST();
    String getCurDate();
    String getCurTimestamp();
    void setClock();

};


#endif
