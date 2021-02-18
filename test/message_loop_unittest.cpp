#include "stdafx_nc_runtime_test.h"
#include "message_loop.h"
#include "nc_string.h"
#include "nc_stdlib.h"
#include <thread>

using namespace nc;

class PrintMessage : public Message {
public:
  static sp<PrintMessage> alloc(NcString* text) {
    auto o = new PrintMessage();
    o->m_text = retain(text);
    return o;
  }
  virtual void run(MessageLoop* loop) override { printf("%s", m_text->cstr()); }

private:
  sp<NcString> m_text;
};

TEST(MessageLoop, basic) {
  MessageLoop* loop = NULL;

  thread t([&loop] {
    loop = MessageLoop::threadInstance();
    loop->run();
  });

  while (loop == NULL) {
  }

  for (size_t i = 0; i < 10; i++) {
    loop->postMessage(PrintMessage::alloc("hello\n"_str));
  }
  Thread::sleep(1000);
  loop->postMessage(nc::QuitMessage::alloc());

  t.join();
}
