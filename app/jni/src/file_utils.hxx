#pragma once



#include <stdbool.h>
#include <vector>
#include <string>
#include <sstream>



#ifndef NON_ANDROID
    #define BASE_DIRECTORY "/sdcard/Download/"
    #define BASE_DIRECTORY_IMAGES "/sdcard/Pictures/"
    #define APP_NAMESPACE "com.qsp"
#endif




class NVFileUtils
{
    public:
    
    typedef struct
    {
        std::string file_name;
        std::string size;
        std::string last_mod;
        //std::string location; // Kind of useless
        bool success; // Checks if the operation completed successfully
        std::ostringstream err; // Inserting errors
    }FileInfo;
    
    
    
    
    static std::string GetFilePathA(std::string filename, const char * reading_mode);
    static std::vector<std::string> GetFilesByExtension(std::string base_dir, std::string f_ext);
    static bool FileExists(std::string f);
    static FileInfo GetFileInfo(std::string f);
};