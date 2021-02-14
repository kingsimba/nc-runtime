#include "stdafx_nc_runtime_test.h"
#include "nc_string.h"

TEST(NcString, basic) {
  auto s = NcString::allocWithCString("hello");
  EXPECT_STREQ(s->cstr(), "hello");
}
