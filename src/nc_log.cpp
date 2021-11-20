#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_log.h"
#include "nc_runtime/nc_stdlib.h"
#include <stdarg.h>

const char* LogLevel_toString(LogLevel l)
{
    static const char* strs[] = {"none", "trivial", "info", "warning", "error", "fatal"};
    if ((int)l >= (int)countof(strs) || (int)l <= 0)
        l = LogLevel::fatal;
    return strs[(int)l];
}

static NcLogCallback g_logCallback = NULL;
static void* g_logUserdata = NULL;
LogLevel g_ncLogLevel = LogLevel::warning;

void NcLog_setCallback(NcLogCallback callback, void* userdata)
{
    g_logCallback = callback;
    g_logUserdata = userdata;
}

void NcLog_write(LogLevel level, const char* file, int line, const char* func, const char* format, ...)
{
    UNUSED_VAR(func);
    const static size_t STR_MAX_LEN = 2048;
    StackOrHeapAllocator allocator(alloca(STR_MAX_LEN * 2), STR_MAX_LEN * 2);

    char* finalFormat = allocator.allocArray<char>(STR_MAX_LEN);
    char* message = allocator.allocArray<char>(STR_MAX_LEN);

    size_t require =
        snprintf(finalFormat, STR_MAX_LEN, "%s(%d): %s: %s\n", file, line, LogLevel_toString(level), format);
    if (require >= STR_MAX_LEN)
    {
        // reallocate memory if it's not enough
        finalFormat = allocator.allocArray<char>(require + 1);
        require = snprintf(finalFormat, require + 1, "%s(%d): %s: %s\n", file, line, LogLevel_toString(level), format);
    }

    va_list va;
    va_start(va, format);
    require = vsnprintf(message, STR_MAX_LEN, finalFormat, va);
    if (require >= STR_MAX_LEN)
    {
        // reallocate memory if it's not enough
        message = allocator.allocArray<char>(require + 1);
        // restart va_list
        va_end(va);
        va_start(va, format);
        require = vsnprintf(message, require + 1, finalFormat, va);
    }
    va_end(va);
    if (g_logCallback != NULL)
    {
        g_logCallback(message, g_logUserdata);
    }
    else
    {
        Dal_writeLog(message);
    }
}

#ifdef NC_OS_WIN
#    include <windows.h>
void Dal_writeLog(const char* str)
{
    printf("%s", str);
    OutputDebugStringA(str);
}

#else
void Dal_writeLog(const char* str)
{
    printf("%s", str);
}
#endif
