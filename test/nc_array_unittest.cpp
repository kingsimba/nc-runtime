#include "stdafx_nc_runtime_test.h"
#include "nc_array.h"
#include "nc_string.h"

using namespace std;

TEST(ArrayTest, vector) {

  sptr<NcString> perm;

  {
    auto str = NcString::allocWithCString("hello world");

    auto v = NcArrayRef<NcString>::alloc();
    v->addObject(str);
    v->addObject(str);
    EXPECT_EQ(str->retainCount(), 3);

    auto pieces = str->split(" ");
    ASSERT_EQ(pieces.size(), 2);
    for (auto s : pieces) {
      v->addObject(NcString::allocWithSlice(s));
    }

    EXPECT_STREQ(v[0]->cstr(), "hello world");
    EXPECT_STREQ(v[1]->cstr(), "hello world");
    EXPECT_STREQ(v[2]->cstr(), "hello");
    EXPECT_STREQ(v[3]->cstr(), "world");

    perm = v[0];
    EXPECT_EQ(perm->retainCount(), 4);
  }

  EXPECT_EQ(perm->retainCount(), 1);
}
