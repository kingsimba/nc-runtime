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
