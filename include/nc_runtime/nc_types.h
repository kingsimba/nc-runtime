#pragma once

#include <stdlib.h>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include <cmath>
#include <new>

#define UNUSED_VAR(o) (void)(o)

//////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#    define NC_OS_WIN
#    define NC_PLATFORM_WIN
#else
#    define NC_OS_UNIX
#endif

#ifdef NC_OS_WIN
#    define forceinline __forceinline
#    pragma warning(disable : 4201)
#    pragma warning(disable : 4714) // warning C4714: function marked as __forceinline not inlined
#else
#    define forceinline inline
#    pragma GCC diagnostic ignored "-Wpedantic" // warning: ISO C++ prohibits anonymous structs [-Wpedantic]
#endif

#if defined(_M_X64) || defined(__amd64__) || defined(__LP64__) || defined(__x86_64__)
#    define NC_ENVIRONMENT_64 1
#else
#    define NC_ENVIRONMENT_32 1
#endif

#ifdef NC_OS_WIN
#    define NC_DEPRECATED __declspec(deprecated)
#else
#    define NC_DEPRECATED __attribute__((deprecated))
#endif

#define NC_MAX_PATH 512

// single precision
#define NC_TWO_PI 6.2831853f // 2 * pi
#define NC_PI 3.14159265f    // pi
#define NC_PI_2 1.57079633f  // pi/2
#define NC_PI_4 0.785398163f // pi/4
#define NC_1_PI 0.318309886f // 1/pi
#define NC_2_PI 0.636619772f // 2/pi

// double precision
#define NC_TWO_PI_D 6.28318530717958647692 // 2 * pi
#define NC_PI_D 3.14159265358979323846     // pi
#define NC_PI_2_D 1.57079632679489661923   // pi/2
#define NC_PI_4_D 0.785398163397448309616  // pi/4
#define NC_1_PI_D 0.318309886183790671538  // 1/pi
#define NC_2_PI_D 0.636619772367581343076  // 2/pi

// clang-format off
static forceinline float nc_radianToDegree(float radian) { return radian * (180.0f / NC_PI); }

static forceinline float nc_degreeToRadian(float degree) { return degree * (NC_PI / 180.0f); }

static forceinline bool nc_isnan(float n) { return n != n; }

template <typename T> forceinline T nc_abs(T o) { return o > 0 ? o : -o; }
template <typename T> forceinline T nc_min(T l, T r) { return l < r ? l : r; }
template <typename T> forceinline T nc_max(T l, T r) { return l > r ? l : r; }
template <typename T> forceinline T nc_clamp(T o, T lower, T upper) { return o < lower ? lower : (o > upper ? upper : o); } // clang-format on

//////////////////////////////////////////////////////////////////////////

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef signed char i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef char32_t wchar32;
typedef size_t Handle;

#define invalidHandle 0
#define countof(o) (sizeof(o) / sizeof((o)[0]))
#ifdef NC_OS_WIN
#    define nc_alloca(o) _alloca(o)
#else
#    define nc_alloca(o) alloca(o)
#endif

//////////////////////////////////////////
// Size

struct Size
{
    int width;
    int height;
};

inline Size Size_make(int w, int h)
{
    return Size{w, h};
}

inline bool operator==(Size l, Size r)
{
    return l.width == r.width && l.height == r.height;
}

inline bool operator!=(Size l, Size r)
{
    return l.width != r.width || l.height != r.height;
}

#include "_rect.h"
#include "_rectf.h"

/////////////////////////////////////////////////
// Rgba8
struct Rgba8
{
    Rgba8() = default;
    forceinline Rgba8(u32 rgba)
    {
        // flip endian
        value = ((rgba << 8) & 0xFF00FF00) | ((rgba >> 8) & 0xFF00FF);
        value = (value << 16) | (value >> 16);
    }
    forceinline Rgba8(u8 r_, u8 g_, u8 b_, u8 a_) : r(r_), g(g_), b(b_), a(a_) {}
    union {
        struct
        {
            u8 r, g, b, a;
        };
        u32 value;
    };
};

NC_DEPRECATED inline Rgba8 Rgba8_make(u8 r, u8 g, u8 b, u8 a)
{
    Rgba8 o(r, g, b, a);
    return o;
}

inline bool operator==(Rgba8 l, Rgba8 r)
{
    return l.value == r.value;
}

inline bool operator!=(Rgba8 l, Rgba8 r)
{
    return l.value != r.value;
}

////////////////////////////////////////////////////////////////////////////
// Range

struct Range
{
    Range() = default;
    Range(int location_, int length_) : location(location_), length(length_) {}

    int location;
    int length;
    forceinline int end() { return this->location + this->length; }
    forceinline void invalidate()
    {
        this->location = -1;
        this->length = 0;
    }
    forceinline bool isValid() { return location >= 0; }
};

#ifdef NC_OS_UNIX
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif
static Range invalidRange{-1, 0};
#ifdef NC_OS_UNIX
#    pragma GCC diagnostic pop
#endif

static forceinline Range Range_make(int location, int length)
{
    return Range{location, length};
}

forceinline bool operator==(const Range& l, const Range& r)
{
    return l.location == r.location && l.length == r.length;
}
forceinline bool operator!=(const Range& l, const Range& r)
{
    return l.location != r.location || l.length != r.length;
}

//////////////////////////////////////////////////////////////////////////

class NoValueType
{
};

const static NoValueType noValue = {};

/**
 * Used for optional return value of function.
 *
 * ```
 * Some<string> getName() {
 *   if (hasError())
 *     return noValue;
 *   else {
 *     return calculateName();
 *   }
 * }
 *
 * auto v = getName().Or("Unnamed User");
 * ```
 */
template <typename T>
class Some
{
public:
    forceinline Some() : m_hasValue(false) {}
    forceinline Some(const NoValueType& v) : m_hasValue(false) { UNUSED_VAR(v); }
    forceinline Some(const T& v) : m_hasValue(true), m_value(v) {}
    forceinline Some(const T&& v) : m_hasValue(true), m_value(std::move(v)) {}

    forceinline T& value() { return m_value; }
    forceinline const T& value() const { return m_value; }
    forceinline bool hasValue() const { return m_hasValue; }

    forceinline const T& Or(const T& r) const { return m_hasValue ? m_value : r; }

private:
    bool m_hasValue;
    T m_value;
};

//////////////////////////////////////////////////////////////////////////

// Learned from https://rigtorp.se/spinlock/
class Spinlock
{
public:
    void lock()
    {
        for (;;)
        {
            if (!m_lock.exchange(true, std::memory_order_acquire))
            {
                break;
            }
            while (m_lock.load(std::memory_order_relaxed))
            {
            }
        }
    }

    void unlock() { m_lock.store(false, std::memory_order_release); }

private:
    std::atomic<bool> m_lock{false};
};
