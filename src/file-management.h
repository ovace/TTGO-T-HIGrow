#pragma once

#ifndef __espFileMgmt_H
#define __espFileMgmt_H

#include <FS.h>

class espFileMgmt {
    public:
        espFileMgmt();
        ~espFileMgmt();            
        static void setupespFileMgmt();
        static void getFileMgmtCfg();
        static void writeFile(fs::FS & fs, const char * path, const char * message);
        static void readFile(fs::FS & fs, const char * path);
        static void listDir(fs::FS & fs, const char * dirname, uint8_t levels);
    private:

};      
#endif