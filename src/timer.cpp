#include "stdafx_nc_runtime.h"
#include "timer.h"
#include "nc_stdlib.h"

Timer::Timer(TimeTick interval, NcObject* target, std::function<bool(NcObject* obj)> func) {
  m_interval = interval;
  m_dueTime = TimeTick::now() + interval;
  m_target = retain(target);
  m_func = func;
}
