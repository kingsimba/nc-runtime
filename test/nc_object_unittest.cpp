#include "stdafx_nc_runtime_test.h"
#include "nc_object.h"

class MyBox : public NcObject {
public:
  static sp<MyBox> alloc() { return std::make_shared<MyBox>(); }

  MyBox(){};
};

TEST(NcObject, cast) {
  auto box = MyBox::alloc();

  // cast to base
  sp<NcObject> base = box;
  EXPECT_EQ(box.use_count(), 2);

  // cast to derived
  auto box2 = std::static_pointer_cast<MyBox>(base);
  EXPECT_EQ(box.use_count(), 3);
  EXPECT_TRUE(box.get() == box2.get());
}
