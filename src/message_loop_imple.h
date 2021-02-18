#pragma once

#include "message_loop.h"
#include "nc_array.h"
#include "timer_manager.h"

namespace nc {

class MessageLoopImple : public MessageLoop {
public:
  MessageLoopImple();

  virtual void postMessage(Message* msg) override;
  virtual void run() override;
  virtual void addTimer(Timer* timer) override;

private:
  sp<NcArray<Message>> m_messages;
  std::recursive_mutex m_messagesMutex;
  ManualResetEvent m_event;
  TimerManager m_timerManager;
};

}  // namespace nc
