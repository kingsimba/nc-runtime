#include "stdafx_nc_runtime.h"
#include "message_loop_imple.h"

namespace nc {

void QuitMessage::run(MessageLoop* loop) { loop->setShouldQuit(); }

//////////////////////////////////////////////////////////////////////////

MessageLoop* MessageLoop::threadInstance() {
  static thread_local MessageLoopImple loop;
  return &loop;
}

MessageLoopImple::MessageLoopImple() { m_messages = NcArray<Message>::alloc(); }

void MessageLoopImple::postMessage(Message* msg) {
  synchroized(m_messages) {
    m_messages->addObject(msg);
    if (m_messages->size() == 1) {
      m_event.set();
    }
  }
}

void MessageLoopImple::run() {
  while (!shouldQuit()) {
    // block until the message queue is not empty
    bool isEmpty = true;
    while (isEmpty) {
      synchroized(m_messages) { isEmpty = m_messages->size() == 0; }

      if (isEmpty) {
        m_event.wait();
      }
    }

    // get first message
    sp<Message> msg;
    synchroized(m_messages) {
      msg = m_messages->firstObject();
      m_messages->removeObjectAtIndex(0);
    }

    // run the message
    msg->run(this);
    msg.reset();
  }
}

void MessageLoopImple::addTimer(Timer* timer) { m_timerManager.addTimer(timer); }

}  // namespace nc
