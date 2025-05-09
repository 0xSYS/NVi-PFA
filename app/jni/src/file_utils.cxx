#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>

#include <SDL3/SDL.h>




#include "file_utils.hxx"




std::vector<std::string> NVFileUtils::GetFilesByExtension(std::string base_dir, std::string f_ext)
{
    std::vector<std::string> result;
        struct dirent* entry;
        DIR* dir;
    
        // Make sure base_dir ends with a '/'
        if (base_dir.back() != '/')
            base_dir += '/';
    
        dir = opendir(base_dir.c_str());
    
        if (!dir)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", ("Failed to scan: " + base_dir).c_str(), nullptr);
            return result;
        }
    
        while ((entry = readdir(dir)) != NULL) 
        {
            std::string name = entry->d_name;
    
            // Skip "." and ".."
            if (name == "." || name == "..")
                continue;
    
            std::string full_path = base_dir + name;
    
            struct stat statbuf;
            if (lstat(full_path.c_str(), &statbuf) == -1)
                continue;
                
                
            if (S_ISLNK(statbuf.st_mode)) {
                // Skip symbolic links entirely to avoid infinite recursion
                continue;
            }
    
            if (S_ISDIR(statbuf.st_mode)) 
            {
                // Recursively scan and collect files from subdirectory
                std::vector<std::string> sub_result = GetFilesByExtension(full_path, f_ext);
                result.insert(result.end(), sub_result.begin(), sub_result.end());
            } 
            else if (S_ISREG(statbuf.st_mode)) 
            {
                if (name.size() >= f_ext.size() &&
                    name.substr(name.size() - f_ext.size()) == f_ext)
                {
                    // std::cout << "Files found: " << full_path << "\n";
                    result.push_back(full_path);
                }
            }
        }
    
        closedir(dir);
        return result;
}

#ifndef NON_ANDROID
std::string NVFileUtils::GetFilePathA(std::string filename, const char * reading_mode)
{
    std::string out_path;
    std::ostringstream full_path;
    char * base_path = SDL_GetPrefPath(APP_NAMESPACE, "nvpfa");
    full_path << base_path << filename;
    SDL_IOStream *check_file = SDL_IOFromFile(filename.c_str(), reading_mode);
    if(check_file != NULL)
    {
        SDL_CloseIO(check_file);
        out_path = full_path.str();
    }
    else
    {
        out_path = "";
        std::ostringstream temp_msg;
        temp_msg << "Failed to get file !\n" << "'" << full_path.str() << "' Does not exists !";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!!!!!", temp_msg.str().c_str(), nullptr);
    }
    SDL_free(base_path);
    return out_path;
}
#endif

bool NVFileUtils::FileExists(std::string f)
{
#ifdef __linux__
    std::ifstream file(f.c_str());
    if(file.fail())
    {
        return false;
    }
    return true;
#endif
}