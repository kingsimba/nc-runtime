#include "stdafx_nc_runtime_test.h"
#include "nc_log.h"
#include "nc_string.h"

TEST(Log, basic) {
  static sp<NcString> message;
  NcLog_setCallback([](const char* str, void*) { message = NcString::allocWithCString(str); }, NULL);
  NC_LOG_ERROR("hello %s", "world");
  EXPECT_TRUE(message->endsWith("error: hello world\n"));
}
