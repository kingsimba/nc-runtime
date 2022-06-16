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

    forceinline void cancel() { m_shouldExit = true; }

    forceinline bool shouldExit() const { return m_shouldExit; }

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

    virtual sp<NcObject> doTask() = 0;

private:
    void doTaskAndGetResult()
    {
        sp<NcObject> result = this->doTask();

        // callback result
        m_delegate(result);

        release(this);
    }

private:
    bool m_shouldExit = false;
    std::thread m_thread;
    // std::future<void> m_asyncTask;
    TaskCallback m_delegate;
};

} // namespace nc
