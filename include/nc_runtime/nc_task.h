#pragma once

#include "nc_object.h"
#include <thread>
#include <functional>
//#include <future>

namespace nc
{

class Task : public NcObject
{
public:
    using TaskCallback = std::function<void(sp<NcObject> result)>;
    Task(TaskCallback d) : m_delegate(d) {}
    ~Task()
    {
        if (m_thread.joinable())
        {
            m_thread.detach();
        }
    }

    forceinline void cancel() { m_cancelled = true; }

    forceinline bool cancelled() const { return m_cancelled; }

    void wait()
    {
        // m_asyncTask.wait();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void start()
    {
        // m_asyncTask = std::async(std::launch::async, &Task::doTaskAndGetResult, retain(this));
        m_thread = std::thread(&Task::doTaskAndGetResult, retain(this));
    }

    /**
     * doTask() should access cancelled() as often as possible and
     * return null when task is cancelled.
     */
    virtual sp<NcObject> doTask() = 0;

private:
    void doTaskAndGetResult()
    {
        sp<NcObject> result = this->doTask();

        // callback result
        if (!m_cancelled && result != nullptr)
        {
            m_delegate(result);
        }

        release(this);
    }

private:
    volatile bool m_cancelled = false;
    std::thread m_thread;
    // std::future<void> m_asyncTask;
    TaskCallback m_delegate;
};

} // namespace nc
