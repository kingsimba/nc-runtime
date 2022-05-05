#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_stdlib.h"
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <regex>

#ifdef NC_OS_WIN
#    include <windows.h>
#    include <processthreadsapi.h>
#endif

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

TimeDuration TimeDuration::makeWithString(const char* str)
{
    std::regex rgx("(([0-9]*[.])?[0-9]+d)?(([0-9]*[.])?[0-9]+h)?(([0-9]*[.])?[0-9]+m)?(([0-9]*[.])?[0-9]+s)?");
    std::cmatch matches;
    if (std::regex_search(str, matches, rgx))
    {
        double d = 0, h = 0, m = 0, s = 0;
        if (matches[1].matched)
        {
            d = atof(matches[1].str().c_str());
        }
        if (matches[3].matched)
        {
            h = atof(matches[3].str().c_str());
        }
        if (matches[5].matched)
        {
            m = atof(matches[5].str().c_str());
        }
        if (matches[7].matched)
        {
            s = atof(matches[7].str().c_str());
        }

        return TimeDuration::makeWithSeconds((d * 24 + h) * 3600 + m * 60 + s);
    }
    else
    {
        // not match
    }

    return TimeDuration(0);
}

void Thread::sleep(TimeDuration duration)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(duration.ms()));
}

#if defined(NC_OS_WIN)
static std::wstring _s2ws(const char* str)
{
    int strLen = (int)strlen(str);
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str, strLen, NULL, 0);
    std::wstring wstrTo(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, strLen, &wstrTo[0], sizeNeeded);
    return wstrTo;
}
#endif

void Thread::setName(const char* name)
{
#if defined(NC_OS_WIN)
    std::wstring wname = _s2ws(name);
    SetThreadDescription(GetCurrentThread(), wname.c_str());
#else
    pthread_setname_np(pthread_self(), name);
#endif
}

void StableRate::sleep()
{
    TimeTick now = TimeTick::now();
    TimeDuration d = m_duration - (now - m_lastFireTime);

    if (d.ms() > 0)
    {
        Thread::sleep(d);
        m_lastFireTime = now + d;
    }
    else
    {
        m_lastFireTime = now;
    }
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
