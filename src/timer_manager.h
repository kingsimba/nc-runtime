#pragma once

#include "nc_array.h"
#include "timer.h"
#include <mutex>

namespace nc {

class TimerManager {
public:
  TimerManager() { m_timers = NcArray<Timer>::alloc(); }
  void addTimer(Timer* timer);
  void removeTimer(Timer* timer);
  void runTimers();

private:
  sp<NcArray<Timer>> m_timers;
  mutex m_mutex;
};

}