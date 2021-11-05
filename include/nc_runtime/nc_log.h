#pragma once

enum class LogLevel
{
    none,
    trivial,
    info,
    warning,
    error,
    fatal
};

extern LogLevel g_ncLogLevel;

extern "C"
{

    typedef void (*NcLogCallback)(const char* str, void* userdata);

    void NcLog_setCallback(NcLogCallback callback, void* userdata);
    static forceinline void NcLog_setLevel(LogLevel level) { g_ncLogLevel = level; }
    void NcLog_write(LogLevel level, const char* file, int line, const char* func, const char* format, ...);

    void Dal_writeLog(const char* str); // (DAL = device adaption layer) device dependent implementation
}

// clang-format off
#define __LOG_IMPLE(level, file, line, func, ...) { if (level >= g_ncLogLevel) NcLog_write(level, file, line, func, __VA_ARGS__); }

#define NC_LOG_TRIVIAL(...) __LOG_IMPLE(LogLevel::trivial, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define NC_LOG_INFO(...) __LOG_IMPLE(LogLevel::info, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define NC_LOG_WARNING(...) __LOG_IMPLE(LogLevel::warning, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define NC_LOG_ERROR(...) __LOG_IMPLE(LogLevel::error, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define NC_LOG_FATAL(...) __LOG_IMPLE(LogLevel::fatal, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#if defined(NDEBUG)
#  define NC_ASSERT(predicate)
#else
#  if defined(NC_OS_UNIX)
#      define NC_ASSERT(predicate) if (!(predicate)) { NcLog_write(LogLevel::fatal, __FILE__, __LINE__, __FUNCTION__, "error: %s", #predicate); __builtin_trap(); }
#  else
#      define NC_ASSERT(predicate) if (!(predicate)) { NcLog_write(LogLevel::fatal, __FILE__, __LINE__, __FUNCTION__, "error: %s", #predicate); __debugbreak(); }
#  endif
#endif

// clang-format on
