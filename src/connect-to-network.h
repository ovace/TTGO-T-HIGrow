#pragma once

#ifndef __espNWconn_H
#define __espNWconn_H

class espNWconn {
    public:
        espNWconn();
        ~espNWconn();            
        static void setupNWconn(); 
        static void getNWconnCfg();
        static void connectToNetwork();
        
    private:
};      
#endif