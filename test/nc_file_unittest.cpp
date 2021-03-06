#include "stdafx_nc_runtime_test.h"
#include "nc_file.h"

TEST(NcFile, read) {
  auto o = NcFile::alloc("test_data/hello.txt"_s, FileOpenFlag::read);
  ASSERT_TRUE(o != NULL);
  EXPECT_EQ(o->length(), 5);
  char buffer[10];
  ASSERT_EQ(o->read(buffer, 10), 5);
  auto str = NcString::allocWithBytes(buffer, 5);
  EXPECT_STREQ(str->cstr(), "hello");
}
