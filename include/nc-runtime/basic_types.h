#pragma once

#include <stdlib.h>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#  define forceinline __forceinline
#else
#  define forceinline inline
#endif

//////////////////////////////////////////////////////////////////////////

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;

typedef char32_t wchar32;

struct Range {
  int location;
  int length;
};

static Range invalidRange{-1, 0};
static forceinline Range Range_make(int location, int length) { return Range{location, length}; }

//////////////////////////////////////////////////////////////////////////

template <typename T>
using sp = std::shared_ptr<T>;
template <typename T>
using svector = std::vector<sp<T>>;

//////////////////////////////////////////////////////////////////////////

class NcString;
