#include "stdafx_nc_runtime.h"
#include "timer_manager.h"

void TimerManager::addTimer(Timer* timer) {
  lock_guard<mutex> guard(m_mutex);
  m_timers->addObject(timer);
}

void TimerManager::removeTimer(Timer* timer) {
  lock_guard<mutex> guard(m_mutex);
  int index = m_timers->indexOfObject(timer);
  if (index != -1) {
    m_timers->removeObjectAtIndex(index);
  }
}

void TimerManager::runTimers() {
  TimeTick now = TimeTick::now();

  auto timers = NcArray<Timer>::alloc();
  {
    lock_guard<mutex> guard(m_mutex);
    for (int i = 0; i < m_timers->size(); i++) {
      Timer* timer = m_timers->objectAtIndex(i);
      if (timer->_timeIsUp(now)) {
        timer->_restart(now);
        timer->fire();
      }
    }
  }
}