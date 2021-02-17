#pragma once

#include <stdlib.h>
#include <vector>
#include <atomic>
#include <functional>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#  define NC_OS_WIN
#else
#  define NC_OS_UNIX
#endif

#ifdef NC_OS_WIN
#  define forceinline __forceinline
#else
#  define forceinline inline
#endif

#if defined(_M_X64) || defined(__amd64__) || defined(__LP64__) || defined(__x86_64__)
#  define NC_ENVIRONMENT_64 1
#else
#  define NC_ENVIRONMENT_32 1
#endif

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

struct Range {
  int location;
  int length;
  forceinline int end() { return this->location + this->length; }
  forceinline bool isValid() { return location >= 0; }
};

static Range invalidRange{-1, 0};
static forceinline Range Range_make(int location, int length) { return Range{location, length}; }

forceinline bool operator==(const Range& l, const Range& r) { return l.location == r.location && l.length == r.length; }
forceinline bool operator!=(const Range& l, const Range& r) { return l.location != r.location || l.length != r.length; }

//////////////////////////////////////////////////////////////////////////

class NcString;
