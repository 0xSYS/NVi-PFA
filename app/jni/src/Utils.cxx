#include <cstdarg>
#include <cstdio>
#include "Utils.hxx"
//#include <bass.h>
#include "extern/audio/bass.h"

#if defined(_WIN32) || defined(_WIN64) /* Setting font colors using the Windows API */

#include <Windows.h>

void NVi::error(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); 
    fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0C); 
    fprintf(stderr, "ERROR: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::warn(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); 
    fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0E); 
    fprintf(stderr, "WARNING: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::info(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); 
    fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0A); 
    fprintf(stderr, "INFO: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

#else /* Setting the terminal font color using the \033 control character */

void NVi::error(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[31mERROR: \033[m", prefix);
    va_start(args, str); 
    vfprintf(stderr, str, args); 
    va_end(args);
}

void NVi::warn(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[33mWARNING: \033[m", prefix);
    va_start(args, str); 
    vfprintf(stderr, str, args); 
    va_end(args);
}

void NVi::info(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[32mINFO: \033[m", prefix);
    va_start(args, str); 
    vfprintf(stderr, str, args); 
    va_end(args);
}

#endif


NVi::RGBAint NVi::Frgba2Irgba(ImVec4& col)
{
    RGBAint res;
    res.r = static_cast<int>(col.x * 255.0f);
    res.g = static_cast<int>(col.y * 255.0f);
    res.b = static_cast<int>(col.z * 255.0f);
    res.a = static_cast<int>(col.w * 255.0f);
    return res;
}

//using namespace NVi;

NVi::nv_ul64 NVi::operator"" _u64be(const char *str, size_t n)
{
    nv_ul64 ans = 0;
    u16_t   sft = 0; 

    while (nv_ul64 ch = str[sft])
    {
        ans |= ch << (sft++ << 3);
    }

    return ans;
}

void NVi::revU16(u16_t &x)
{
    x = x >> 8 | x << 8;
}

void NVi::revU32(u32_t &x)
{
    x = x >> 24 | (x & 0xFF0000) >> 8 | (x & 0xFF00) << 8 | x << 24;
}
