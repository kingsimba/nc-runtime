#include "stdafx_nc_runtime_test.h"
#include "nc_data.h"

TEST(NcData, basic) {
  auto o = NcData::allocWithContentsOfFile("test_data/hello.txt"_s);
  EXPECT_EQ(o->length(), 5);
  EXPECT_TRUE(memcmp(o->bytes(), "hello", 5) == 0);

  o = NcData::allocWithBytes("top", 4);
  EXPECT_EQ(o->length(), 4);
  EXPECT_STREQ((char*)o->bytes(), "top");
}
