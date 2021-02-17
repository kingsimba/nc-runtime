#include "stdafx_nc_runtime_test.h"
#include "nc_array.h"
#include "nc_string.h"

using namespace std;

TEST(ArrayTest, vector) {

  sp<NcString> perm;

  {
    auto str = NcString::allocWithCString("hello world");

    auto v = NcArray<NcString>::alloc();
    v->addObject(str);
    v->addObject(str);
    EXPECT_EQ(str->retainCount(), 3);

    auto pieces = str->split(" ");
    EXPECT_EQ(str->retainCount(), 5); // +2 because slices holds reference to the string
    ASSERT_EQ(pieces.size(), 2);
    for (auto s : pieces) {
      v->addObject(NcString::allocWithSlice(s));
    }

    EXPECT_STREQ(v[0]->cstr(), "hello world");
    EXPECT_STREQ(v[1]->cstr(), "hello world");
    EXPECT_STREQ(v[2]->cstr(), "hello");
    EXPECT_STREQ(v[3]->cstr(), "world");

    perm = v[0];  // +1
    EXPECT_EQ(perm->retainCount(), 6);
  }

  EXPECT_EQ(perm->retainCount(), 1);
}

TEST(ArrayTest, find) {
    auto v = NcArray<NcString>::alloc();
    v->addObject(NcString::allocWithCString("hello"));
    v->addObject(NcString::allocWithCString("world"));

    auto startWord = "w"_s;
    auto obj = v->find([&](NcString* v) {
      return v->startsWith(startWord);
    });

    ASSERT_TRUE(obj != NULL);
    EXPECT_STREQ(obj->cstr(), "world");

    obj = v->find([](NcString* v) {
      return v->startsWith("s");
    });

    ASSERT_TRUE(obj == NULL);
}
