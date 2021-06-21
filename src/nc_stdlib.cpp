#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_stdlib.h"
#include <chrono>
#include <mutex>
#include <condition_variable>

TimeTick TimeTick::now()
{
    using namespace std::chrono;
    i64 n = (i64)(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    return TimeTick{n};
}

void Thread::sleep(TimeTick tick)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(tick.ms()));
}

u32 Math_hashString(const char* str)
{
    u32 hash = 5381;
    int c;

    while ((c = *(u8*)(str)++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

u32 Math_hashU32(u32 x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

u64 Math_hashU64(u64 x)
{
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}

size_t Math_hashSizeT(size_t o)
{
#if defined(NC_ENVIRONMENT_64)
    return Math_hashU64(o);
#else
    return Math_hashU32(o);
#endif
}

void* StackOrHeapAllocator::alloc(size_t requirement)
{
    void* rtn;
    if (m_usedStackSize + requirement <= m_stackSize)
    {
        rtn = m_stackBuffer + m_usedStackSize;
        m_usedStackSize += requirement;
    }
    else if (m_heapPointerCount < countof(m_heapPointers))
    {
        rtn = malloc(requirement);
        m_heapPointers[m_heapPointerCount++] = rtn;
    }
    else
    {
        if (m_moreHeapPointers == NULL)
        {
            m_moreHeapPointers = new std::vector<void*>();
            m_moreHeapPointers->reserve(32);
        }
        rtn = malloc(requirement);
        m_moreHeapPointers->push_back(rtn);
    }

    return rtn;
}

StackOrHeapAllocator::~StackOrHeapAllocator()
{
    for (size_t i = 0; i < m_heapPointerCount; i++)
    {
        free(m_heapPointers[i]);
    }
    if (m_moreHeapPointers != NULL)
    {
        for (size_t i = 0; i < m_moreHeapPointers->size(); i++)
        {
            free(m_moreHeapPointers->at(i));
        }
        delete m_moreHeapPointers;
    }
}

class ManualResetEventImple
{
  public:
    ManualResetEventImple(bool signaled) : m_signaled(signaled) {}

    void set()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_signaled = true;
        }

        // Notify all because until the event is manually
        // reset, all waiters should be able to see event signaling
        m_cv.notify_all();
    }

    void unset()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_signaled = false;
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_signaled)
        {
            m_cv.wait(lock);
        }
    }

    bool waitWithTimeout(const TimeTick& t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_signaled)
        {
            std::chrono::system_clock::time_point until =
                std::chrono::system_clock::now() + std::chrono::milliseconds(t.ms());
            if (m_cv.wait_until(lock, until) == cv_status::timeout)
            {
                return false;
            }
        }

        return true;
    }

  private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_signaled;
};

ManualResetEvent::ManualResetEvent(bool signaled /*= false*/)
{
    m_imple = new ManualResetEventImple(signaled);
}
ManualResetEvent::~ManualResetEvent()
{
    delete m_imple;
}
void ManualResetEvent::set()
{
    m_imple->set();
}
void ManualResetEvent::reset()
{
    m_imple->unset();
}
void ManualResetEvent::wait()
{
    m_imple->wait();
}
bool ManualResetEvent::waitWithTimeout(const TimeTick& t)
{
    return m_imple->waitWithTimeout(t);
}
