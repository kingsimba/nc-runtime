#pragma once

#include "basic_types.h"

u32 Math_hashString(const char* str);
u32 Math_hashU32(u32 o);
u64 Math_hashU64(u64 o);
size_t Math_hashSizeT(size_t o);

/**
 * Allocate memory on the stack or heap.
 * 
 * Use stack if possible. But if stack is exhausted, use heap.
 * All allocated memory will be freed in destructor.
 * 
 * ```
 * StackOrHeapAllocator allocator(alloca(1024), 1024);
 * allocator.allocArray<char>(512); // from stack
 * allocator.allocArray<char>(512); // from stack
 * allocator.allocArray<char>(1); // from heap
 * ```
 */
class StackOrHeapAllocator {
public:
  StackOrHeapAllocator(void* stackBuffer, size_t stackSize) {
    m_stackBuffer = (u8*)stackBuffer;
    m_stackSize = stackSize;
    m_usedStackSize = 0;
    m_heapPointers[0] = NULL;
    m_heapPointerCount = 0;
    m_moreHeapPointers = NULL;
  }

  ~StackOrHeapAllocator();

  void* alloc(size_t count);

  template <typename T>
  T* allocArray(size_t count) {
    return (T*)alloc(sizeof(T) * count);
  }

  // for testing
  size_t _moreHeapPointerCount() { return m_moreHeapPointers == NULL ? 0 : m_moreHeapPointers->size(); }

private:
  u8* m_stackBuffer;
  size_t m_stackSize;
  size_t m_usedStackSize;
  void* m_heapPointers[8];
  size_t m_heapPointerCount;
  std::vector<void*>* m_moreHeapPointers;
};
