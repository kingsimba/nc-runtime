#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_stdlib.h"
#include <chrono>
#include <mutex>
#include <condition_variable>

char* nc_strtok(char* s, const char* delim, char** savePtr)
{
    char* end;
    if (s == NULL)
        s = *savePtr;
    if (*s == '\0')
    {
        *savePtr = s;
        return NULL;
    }
    /* Scan leading delimiters.  */
    s += strspn(s, delim);
    if (*s == '\0')
    {
        *savePtr = s;
        return NULL;
    }
    /* Find the end of the token.  */
    end = s + strcspn(s, delim);
    if (*end == '\0')
    {
        *savePtr = end;
        return s;
    }
    /* Terminate the token and make *SAVE_PTR point past it.  */
    *end = '\0';
    *savePtr = end + 1;
    return s;
}

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

class ResetableEventImple
{
public:
    ResetableEventImple(bool signaled) : m_signaled(signaled) {}

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

ResetableEvent::ResetableEvent(bool signaled /*= false*/, bool autoReset)
{
    UNUSED_VAR(autoReset);
    m_imple = new ResetableEventImple(signaled);
}
ResetableEvent::~ResetableEvent()
{
    delete m_imple;
}
void ResetableEvent::set()
{
    m_imple->set();
}
void ResetableEvent::reset()
{
    m_imple->unset();
}
void ResetableEvent::wait()
{
    m_imple->wait();
}
bool ResetableEvent::waitWithTimeout(const TimeTick& t)
{
    return m_imple->waitWithTimeout(t);
}
