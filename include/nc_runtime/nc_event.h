#pragma once

#include "nc_stdlib.h"

class ResetableEventImple;

class ResetableEvent
{
public:
    ResetableEvent(bool signaled = false, bool autoReset = false);
    ~ResetableEvent();

    void set();
    void reset();
    void wait();
    bool waitWithTimeout(const TimeTick& t);

private:
    ResetableEventImple* m_imple;
};
