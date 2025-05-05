#pragma once



#include <stdbool.h>
#include <vector>
#include <string>



#ifndef NON_ANDROID
    #define BASE_DIRECTORY "/sdcard/Download/"
    #define APP_NAMESPACE "com.qsp"
#endif




class NVFileUtils
{
    public:
    static std::string GetFilePathA(std::string filename, const char * reading_mode);
    static std::vector<std::string> GetFilesByExtension(std::string base_dir, std::string f_ext);
    static bool FileExists(std::string f);
};