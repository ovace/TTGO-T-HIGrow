#pragma once

#ifndef __saveCfg_H
#define __saveCfg_H

class saveCfg {
    public:
        saveCfg();
        ~saveCfg();  
        static void setupsaveCfg();
        static void getsaveCfg();          
        static void saveConfiguration(const GlobalConfig & config);        
    private:

};      
#endif /* __saveCfg_H */