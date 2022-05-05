#pragma once

#include "nc_types.h"
#include "nc_object.h"
#include "nc_log.h"

// see GNU strtok_r()
char* nc_strtok(char* s, const char* delim, char** savePtr);

forceinline u16 nc_ntohs(u16 val)
{
    val = (val << 8) | (val >> 8);
    return val;
}

forceinline u32 nc_ntohl(u32 val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

forceinline void nc_flipU16(u16& val)
{
    val = (val << 8) | (val >> 8);
}

forceinline void nc_flipU32(u32& val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    val = (val << 16) | (val >> 16);
}

forceinline void nc_flipI16(i16& val)
{
    val = (i16)((u16)(val) << 8) | ((u16)(val) >> 8);
}

forceinline void nc_flipI32(i32& val)
{
    val = (i32)((((u32)(val) << 8) & 0xFF00FF00) | (((u32)(val) >> 8) & 0xFF00FF));
    val = (i32)(((u32)(val) << 16) | ((u32)(val) >> 16));
}

/////////////////////////////////////////////////////////////////
// TimeTick

struct TimeDuration
{
    forceinline TimeDuration() = default;
    forceinline TimeDuration(i64 ms) : __duration(ms) {}

    /**
     * @param str
     *   something like "5.3s", "3d", "2.5d", "6h", "1m", "60s", "1m30s", "3d6h3m20s"
     */
    static TimeDuration makeWithString(const char* str);
    forceinline static TimeDuration makeWithSeconds(float seconds) { return TimeDuration(i64(seconds * 1000 + 0.5f)); }
    forceinline static TimeDuration makeWithSeconds(double seconds) { return TimeDuration(i64(seconds * 1000 + 0.5)); }

    const TimeDuration& operator+=(const TimeDuration& d)
    {
        __duration += d.__duration;
        return *this;
    }

    const TimeDuration& operator-=(const TimeDuration& d)
    {
        __duration -= d.__duration;
        return *this;
    }

    forceinline i64 ms() const { return __duration; }

    i64 __duration = 0;
};

forceinline bool operator==(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration == r.__duration;
}
forceinline bool operator!=(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration != r.__duration;
}
forceinline TimeDuration operator+(const TimeDuration& l, const TimeDuration& r)
{
    return TimeDuration(l.__duration + r.__duration);
}
forceinline TimeDuration operator-(const TimeDuration& l, const TimeDuration& r)
{
    return TimeDuration(l.__duration - r.__duration);
}
forceinline bool operator<(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration < r.__duration;
}
forceinline bool operator<=(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration <= r.__duration;
}
forceinline bool operator>(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration > r.__duration;
}
forceinline bool operator>=(const TimeDuration& l, const TimeDuration& r)
{
    return l.__duration >= r.__duration;
}

// in milliseconds
struct TimeTick
{
    static TimeTick now();

    // return the time used to run some code
    template <typename Func>
    static TimeDuration measure(Func func);

    forceinline TimeTick() : __time(0) {}
    forceinline TimeTick(i64 ms) : __time(ms) {}

    forceinline TimeTick& operator+=(const TimeDuration& r)
    {
        __time += r.__duration;
        return *this;
    }

    forceinline TimeTick& operator-=(const TimeDuration& r)
    {
        __time -= r.__duration;
        return *this;
    }

    i64 __time; // in milliseconds
};

forceinline bool operator==(const TimeTick& l, const TimeTick& r)
{
    return l.__time == r.__time;
}
forceinline bool operator!=(const TimeTick& l, const TimeTick& r)
{
    return l.__time != r.__time;
}
forceinline bool operator<(const TimeTick& l, const TimeTick& r)
{
    return l.__time < r.__time;
}
forceinline bool operator<=(const TimeTick& l, const TimeTick& r)
{
    return l.__time <= r.__time;
}
forceinline bool operator>(const TimeTick& l, const TimeTick& r)
{
    return l.__time > r.__time;
}
forceinline bool operator>=(const TimeTick& l, const TimeTick& r)
{
    return l.__time >= r.__time;
}
forceinline TimeTick operator-(const TimeTick& l, const TimeDuration& duration)
{
    return TimeTick{l.__time - duration.__duration};
}
forceinline TimeTick operator+(const TimeTick& l, const TimeDuration& duration)
{
    return TimeTick{l.__time + duration.__duration};
}
forceinline TimeDuration operator-(const TimeTick& l, const TimeTick& r)
{
    return TimeDuration{l.__time - r.__time};
}
forceinline TimeDuration operator+(const TimeTick& l, const TimeTick& r)
{
    return TimeDuration{l.__time + r.__time};
}

template <typename Func>
TimeDuration TimeTick::measure(Func func)
{
    TimeTick start = TimeTick::now();
    func();
    TimeDuration elapsed = TimeTick::now() - start;
    return elapsed;
}

/**
 * Loop rate control
 *
 * ```
 * StableRate rate(10);
 * TimeTick startTime = TimeTick::now();
 * int count = 0;
 * while (TimeTick::now() - startTime < TimeDuration(1000))
 * {
 *     count++;
 *     rate.sleep();
 * }
 * EXPECT_EQ(count, 10);
 * ```
 */
class StableRate
{
public:
    StableRate(float hz) : m_duration((i64)(1000 / hz)) { m_lastFireTime = TimeTick::now(); }

    void sleep();

private:
    TimeDuration m_duration;
    TimeTick m_lastFireTime;
};

/////////////////////////////////////////////////////////////////
// Thread

class Thread
{
public:
    static void sleep(TimeDuration tick); // sleep current thread for at least some time.
    static void setName(const char* name); // set the name of current thread
};

/**
 * Allocate memory on the stack or heap.
 *
 * Use stack if possible. But if stack is exhausted, use heap.
 * All allocated memory will be freed in destructor.
 *
 * ```
 * StackOrHeapAllocator allocator(nc_alloca(1024), 1024);
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

    template <typename T>
    T* allocArray(size_t count)
    {
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

class StandardFreer
{
public:
    StandardFreer(void* p) { m_p = p; }
    ~StandardFreer() { free(m_p); }

    void release() { m_p = NULL; }

private:
    void* m_p;
};

template <typename T>
inline T* nc_copyArray(const T* arr, size_t count)
{
    size_t totalSize = sizeof(T) * count;
    T* copy = (T*)malloc(totalSize);
    memcpy(copy, arr, totalSize);
    return copy;
}

//////////////////////////////////////////////////////////////////////////

class MachineState : public NcObject
{
public:
    virtual void stateBegin() = 0;
    virtual void stateUpdate() = 0;
    virtual void stateEnd() = 0;
};

class StateMachine : public NcObject
{
public:
    StateMachine() {}
    ~StateMachine()
    {
        if (m_currentState)
            m_currentState->stateEnd();
    }

    void gotoState(sp<MachineState> state)
    {
        if (state == m_currentState)
            return;

        if (m_currentState)
            m_currentState->stateEnd();
        state->stateBegin();
        m_currentState = state;
    }

    void spinOnce() { m_currentState->stateUpdate(); }

private:
    sp<MachineState> m_currentState;
};
