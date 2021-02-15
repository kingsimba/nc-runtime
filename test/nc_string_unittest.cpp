#include "stdafx_nc_runtime_test.h"
#include "nc_string.h"

static int _calculateStringLength(NcString* str) { return str->length(); }

TEST(NcString, basic) {
  sptr<NcString> s = NcString::allocWithCString("hello");
  EXPECT_STREQ(s->cstr(), "hello");
  EXPECT_EQ(s->length(), 5);

  // must call .get() to convert to ordinary pointer.
  EXPECT_EQ(_calculateStringLength(s), 5);

  s = NcString::allocWithBytes("cat litter", 10);
  EXPECT_STREQ(s->cstr(), "cat litter");
  EXPECT_EQ(s->length(), 10);

  s = NcString::allocWithSlice(StringSlice("dog food"));
  EXPECT_STREQ(s->cstr(), "dog food");
  EXPECT_EQ(s->length(), 8);
}

TEST(NcString, join) {
  sptr<NcString> s = NcString::allocByJoiningSlices(vector<StringSlice>{"hello", "world"}, "---");
  EXPECT_STREQ(s->cstr(), "hello---world");
  EXPECT_EQ(s->length(), 13);
}

//TEST(NcString, split) {
//  // ERROR: slices contains invalid memory.
//  // Because the smart point created with NcString::allocWithCString() is freed
//  vector<StringSlice> slices = NcString::allocWithCString("hello---world")->split("---");
//  auto s = NcString::allocByJoiningSlices(slices, " ");
//  EXPECT_STREQ(s->cstr(), "hello world");
//  EXPECT_EQ(s->length(), 11);
//}
