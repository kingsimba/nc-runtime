#include "stdafx_nc_runtime.h"
#include "nc_stdlib.h"

u32 Math_hashString(const char* str) {
  u32 hash = 5381;
  int c;

  while (c = *(u8*)(str)++) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

u32 Math_hashU32(u32 x) {
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

u64 Math_hashU64(u64 x) {
  x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
  x = x ^ (x >> 31);
  return x;
}

size_t Math_hashSizeT(size_t o) {
#if defined(NC_ENVIRONMENT_64)
  return Math_hashU64(o);
#else
  return Math_hashU32(o);
#endif
}
