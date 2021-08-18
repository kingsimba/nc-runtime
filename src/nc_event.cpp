#include "stdafx_nc_runtime.h"
#include "nc_runtime/nc_stdlib.h"
#include <chrono>
#include <mutex>
#include <condition_variable>

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
