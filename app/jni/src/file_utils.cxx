#include <SDL3/SDL_filesystem.h>
#include <stdio.h>
#ifdef __linux__
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <pwd.h>
    #include <limits.h>
    #include <ctime>
    #include <iomanip>
#endif

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#endif

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include <SDL3/SDL.h>




#include "file_utils.hxx"
#include "Utils.hxx"






#if defined(_WIN32) || defined(_WIN64)
std::string JoinPath(const std::string& dir, const std::string& file) 
{
    if (dir.empty()) return file;
    char sep = dir.back() == '\\' ? '\0' : '\\';
    return sep ? (dir + sep + file) : (dir + file);
}
#endif

std::vector<std::string> NVFileUtils::GetFilesByExtension(std::string base_dir, std::string f_ext)
{
    std::vector<std::string> result;
#ifdef __linux__
    struct dirent* entry;
    DIR* dir;
    
    // Make sure base_dir ends with a '/'
    if (base_dir.back() != '/')
        base_dir += '/';

    dir = opendir(base_dir.c_str());
    
    if(!dir)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "File Scanner Error!!!!!", ("Failed to scan: " + base_dir).c_str(), nullptr);
        return result;
    }
    
    while((entry = readdir(dir)) != NULL) 
    {
        std::string name = entry->d_name;

        // Skip "." and ".."
        if(name == "." || name == "..")
            continue;

        std::string full_path = base_dir + name;

        struct stat statbuf;
        if(lstat(full_path.c_str(), &statbuf) == -1)
            continue;
            
            
        if(S_ISLNK(statbuf.st_mode)) {
            // Skip symbolic links entirely to avoid infinite recursion
            continue;
        }

        if(S_ISDIR(statbuf.st_mode)) 
        {
            // Recursively scan and collect files from subdirectory
            std::vector<std::string> sub_result = GetFilesByExtension(full_path, f_ext);
            result.insert(result.end(), sub_result.begin(), sub_result.end());
        } 
        else if(S_ISREG(statbuf.st_mode)) 
        {
            if(name.size() >= f_ext.size() && name.substr(name.size() - f_ext.size()) == f_ext)
            {
                // std::cout << "Files found: " << full_path << "\n";
                result.push_back(full_path);
            }
        }
    }
    
    closedir(dir);
#endif

#if defined(_WIN32) || defined(_WIN64)
    if (!base_dir.empty() && base_dir.back() != '\\')
        base_dir += '\\';

    std::string search_path = base_dir + "*";

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(search_path.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        MessageBoxA(NULL, ("Failed to scan: " + base_dir).c_str(), "Error!!!!!", MB_ICONERROR | MB_OK);
        return result;
    }

    do
    {
        std::string name = findFileData.cFileName;

        // Skip "." and ".."
        if(name == "." || name == "..")
            continue;

        std::string full_path = base_dir + name;

        if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            // Skip symbolic links or junctions
            continue;
        }

        if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Recurse into subdirectory
            std::vector<std::string> sub_result = GetFilesByExtension(full_path, f_ext);
            result.insert(result.end(), sub_result.begin(), sub_result.end());
        }
        else if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL || findFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        {
            if(name.size() >= f_ext.size() && name.substr(name.size() - f_ext.size()) == f_ext)
            {
                result.push_back(full_path);
            }
        }

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
#endif
    return result;
}

std::string human_readable_size(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && unit_index < 5) {
        size /= 1024;
        ++unit_index;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

NVFileUtils::FileInfo NVFileUtils::GetFileInfo(std::string f)
{
    NVFileUtils::FileInfo result;
    char abs_path[PATH_MAX];
#ifdef __linux__
    struct stat f_stat;
    std::time_t t;
    std::tm* tm_ptr;
    
    // Get basic file information and populate the the struct members
    if(stat(f.c_str(), &f_stat) == 0)
    {
        t = f_stat.st_mtime;
        tm_ptr = std::localtime(&t);
        result.file_name = f;
        
        std::ostringstream temp;
        temp << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
        result.last_mod  = temp.str();
        result.size      = human_readable_size(f_stat.st_size);
        result.success = true;
    }
    else
    {
        result.success = false;
        result.err << "File does not exist !!\n";
        NVi::error("file_utils", result.err.str().c_str());
    }
#endif

#if defined(_WIN32) || defined(_WIN64)
    // Todo...
#endif
    
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
        temp_msg << "Failed to get asset file !\n" << "'" << full_path.str() << "' Does not exists !";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Asset Error!!!!!", temp_msg.str().c_str(), nullptr);
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

#if defined(_WIN32) || defined(_WIN64)
    std::wstring wstr(f.begin(), f.end());
    LPCWSTR temp = wstr.c_str();
    DWORD fileAttr = GetFileAttributesW(temp);
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
#endif
}