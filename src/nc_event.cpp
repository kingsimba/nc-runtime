/*
MIT License

Copyright (c) 2021 Zhaolin Feng (kingsimba)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_event.h"
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

class ResetableEventImple
{
public:
    ResetableEventImple(bool signaled, bool autoReset)
    {
        m_signaled = signaled;
        m_autoReset = autoReset;
    }

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

    void reset()
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
        if (m_autoReset)
            m_signaled = false;
    }

    EventWaitResult waitWithTimeout(const TimeDuration& t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_signaled)
        {
            std::chrono::steady_clock::time_point until =
                std::chrono::steady_clock::now() + std::chrono::milliseconds(t.ms());
            if (m_cv.wait_until(lock, until) == cv_status::timeout)
            {
                return EventWaitResult::timeout;
            }
        }

        if (m_autoReset)
            m_signaled = false;

        return EventWaitResult::succ;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_signaled;
    bool m_autoReset;
};

ResetableEvent::ResetableEvent(bool signaled, bool autoReset)
{
    m_imple = std::make_unique<ResetableEventImple>(signaled, autoReset);
}
ResetableEvent::~ResetableEvent()
{
}
void ResetableEvent::set()
{
    m_imple->set();
}
void ResetableEvent::reset()
{
    m_imple->reset();
}
void ResetableEvent::wait()
{
    m_imple->wait();
}

EventWaitResult ResetableEvent::waitWithTimeout(const TimeDuration& t)
{
    return m_imple->waitWithTimeout(t);
}
