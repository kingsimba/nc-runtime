#pragma once

#include "nc_object.h"

namespace nc {

class Timer : public NcObject {
public:
  static sp<Timer> alloc(TimeTick interval, NcObject* target, std::function<bool(NcObject* obj)> func) {
    return new Timer(interval, target, func);
  }

  forceinline bool _timeIsUp(TimeTick now) { return (now - m_dueTime).ms() >= 0; }
  forceinline void _restart(TimeTick now) { m_dueTime = now + m_interval; }

  void fire() { m_func(m_target); }

private:
  Timer(TimeTick interval, NcObject* target, std::function<bool(NcObject* obj)> func);
  ~Timer() {}

private:
  TimeTick m_interval;
  TimeTick m_dueTime;
  sp<NcObject> m_target;
  std::function<bool(NcObject* obj)> m_func;
};

}  // namespace nc