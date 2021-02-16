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

TEST(NcString, literal) {
  sptr<NcString> s1, s2;
  for (int i = 0; i < 2; i++) {
    sptr<NcString> s = "hello world"_s;
    if (i == 0)
      s1 = s;
    else
      s2 = s;
  }

  // s1 is exactly the same as s2, because of the literal string manager.
  EXPECT_EQ(s1.get(), s2.get());
  
  EXPECT_EQ(s1->retainCount(), INT_MAX);
  EXPECT_EQ(retain<NcString>(s1)->retainCount(), INT_MAX);
  release(s1);
  EXPECT_EQ(s1->retainCount(), INT_MAX);

  auto s3 = "hello world"_s;
  // for s1 == s3, it must be compiled with /GF(enable string pool) for Visual Studio
  EXPECT_EQ(s1.get(), s3.get());
}

TEST(NcString, join) {
  sptr<NcString> s = NcString::allocByJoiningSlices(vector<StringSlice>{"hello", "world"}, "---");
  EXPECT_STREQ(s->cstr(), "hello---world");
  EXPECT_EQ(s->length(), 13);
}

TEST(NcString, toSlice) {
  sptr<NcString> s = NcString::allocWithCString("hello world");
  auto slice = s->toSlice();
  EXPECT_EQ(s->retainCount(), 2); // slice holds a reference to the string
}

TEST(NcString, split) {
  vector<StringSlice> slices = NcString::allocWithCString("hello---world")->split("---");
  auto s = NcString::allocByJoiningSlices(slices, " ");
  EXPECT_STREQ(s->cstr(), "hello world");
  EXPECT_EQ(s->length(), 11);
}

TEST(NcString, subslice) {
  auto str = NcString::allocWithCString("hello---world");
  auto slice = str->subslice(5, 2);
  EXPECT_EQ(str->retainCount(), 2);
  EXPECT_TRUE(slice.equals("--"));
}
