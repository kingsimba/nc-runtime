#include "stdafx_nc_runtime_test.h"
#include "nc_array.h"
#include "nc_string.h"

using namespace std;

 TEST(ArrayTest, vector) {
  auto str = NcString::allocWithCString("hello world");

  // cast to base
  sp<NcObject> base = str;

  // cast to derived
  str = static_pointer_cast<NcString>(base);

  MyArrayRef<NcString> v = MyArray<NcString>::alloc();
  v->addObject(str);
  v->addObject(str);

  auto pieces = str->split(" ");
  ASSERT_EQ(pieces.size(), 2);
  for (auto s : pieces) {
    v->addObject(NcString::allocWithSlice(s));
  }

  EXPECT_STREQ(v[2]->cstr(), "hello");
  EXPECT_STREQ(v[3]->cstr(), "world");
}
