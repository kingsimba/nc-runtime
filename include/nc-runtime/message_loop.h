#pragma once

#include "nc_object.h"

namespace nc {

class Message : public NcObject {
public:
  virtual void run(MessageLoop* loop) = 0;
};

class QuitMessage : public Message {
public:
  static sp<QuitMessage> alloc() { return new QuitMessage(); }
  virtual void run(MessageLoop* loop);
};

class MessageLoop {
public:
  MessageLoop() { m_shouldQuit = false; }

  static MessageLoop* threadInstance();

  virtual void postMessage(Message* msg) = 0;

  // run the loop until shouldQuit() is true.
  virtual void run() = 0;

  // add a timer into the loop
  virtual void addTimer(Timer* timer) = 0;

  void setShouldQuit() { m_shouldQuit = true; }
  bool shouldQuit() { return m_shouldQuit; }

private:
  bool m_shouldQuit;
};

}  // namespace nc
