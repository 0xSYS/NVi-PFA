#pragma once



#include <stdbool.h>
#include <vector>
#include <string>



#ifdef NON_ANDROID
    #define BASE_DIRECTORY "/home/0xsys/Desktop"
#else
    #define BASE_DIRECTORY "/sdcard/Download"
#endif




class NVFileUtils
{
    public:
    static std::vector<std::string> GetFilesByExtension(std::string base_dir, std::string f_ext);
    static bool FileExists(std::string f);
};