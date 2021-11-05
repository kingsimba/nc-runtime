#pragma once

#include "nc_stdlib.h"

class ResetableEventImple;

enum class EventWaitResult
{
    succ = 0,
    timeout = 1
};

class ResetableEvent
{
public:
    ResetableEvent(bool signaled = false, bool autoReset = false);
    ~ResetableEvent();

    void set();
    void reset();
    void wait();
    EventWaitResult waitWithTimeout(const TimeTick& t);

private:
    std::unique_ptr<ResetableEventImple> m_imple;
};
