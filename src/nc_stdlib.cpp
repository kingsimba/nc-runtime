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

void* StackOrHeapAllocator::alloc(size_t requirement) {
  void* rtn;
  if (m_usedStackSize + requirement <= m_stackSize) {
    rtn = m_stackBuffer + m_usedStackSize;
    m_usedStackSize += requirement;
  } else if (m_heapPointerCount < countof(m_heapPointers)) {
    rtn = malloc(requirement);
    m_heapPointers[m_heapPointerCount++] = rtn;
  } else {
    if (m_moreHeapPointers == NULL) {
      m_moreHeapPointers = new std::vector<void*>();
      m_moreHeapPointers->reserve(32);
    }
    rtn = malloc(requirement);
    m_moreHeapPointers->push_back(rtn);
  }

  return rtn;
}

StackOrHeapAllocator::~StackOrHeapAllocator() {
  for (size_t i = 0; i < m_heapPointerCount; i++) {
    free(m_heapPointers[i]);
  }
  if (m_moreHeapPointers != NULL) {
    for (size_t i = 0; i < m_moreHeapPointers->size(); i++) {
      free(m_moreHeapPointers->at(i));
    }
    delete m_moreHeapPointers;
  }
}
