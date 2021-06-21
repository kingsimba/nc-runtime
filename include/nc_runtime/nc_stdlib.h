#pragma once

#include "nc_types.h"
#include "nc_object.h"
#include "nc_log.h"

// in milliseconds
struct TimeTick
{
    static TimeTick now();

    // return the time used to run some code
    template <typename Func> static TimeTick measure(Func func);

    TimeTick() : __time(0) {}
    TimeTick(i64 ms) { __time = ms; }
    forceinline i64 ms() const { return __time; }

    i64 __time; // in milliseconds
};

forceinline TimeTick operator-(const TimeTick& l, const TimeTick& r)
{
    return TimeTick{l.__time - r.__time};
}
forceinline TimeTick operator+(const TimeTick& l, const TimeTick& r)
{
    return TimeTick{l.__time + r.__time};
}

class Thread
{
  public:
    static void sleep(TimeTick tick); // sleep current thread for at least some time.
};

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
class StackOrHeapAllocator
{
  public:
    StackOrHeapAllocator(void* stackBuffer, size_t stackSize)
    {
        m_stackBuffer = (u8*)stackBuffer;
        m_stackSize = stackSize;
        m_usedStackSize = 0;
        m_heapPointers[0] = NULL;
        m_heapPointerCount = 0;
        m_moreHeapPointers = NULL;
    }

    ~StackOrHeapAllocator();

    void* alloc(size_t count);

    template <typename T> T* allocArray(size_t count) { return (T*)alloc(sizeof(T) * count); }

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

class ManualResetEventImple;

class ManualResetEvent
{
  public:
    ManualResetEvent(bool signaled = false);
    ~ManualResetEvent();

    void set();
    void reset();
    void wait();
    bool waitWithTimeout(const TimeTick& t);

  private:
    ManualResetEventImple* m_imple;
};

class __MutexGuard__
{
  public:
    __MutexGuard__(std::recursive_mutex& m) : m_guard(m) { m_firstTime = true; }

    bool next()
    {
        if (!m_firstTime)
            return false;
        m_firstTime = false;
        return true;
    }

  private:
    std::lock_guard<std::recursive_mutex> m_guard;
    bool m_firstTime;
};

// clang-format off
#define synchronized(o) for(__MutexGuard__ guard(o##Mutex); guard.next(); )
// clang-format on

template <typename Func> TimeTick TimeTick::measure(Func func)
{
    TimeTick start = TimeTick::now();
    func();
    TimeTick elapsed = TimeTick::now() - start;
    return elapsed;
}
