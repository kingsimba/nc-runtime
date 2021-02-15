#include "stdafx_nc_runtime_test.h"
#include "nc_object.h"

class MyBox : public NcObject {
public:
  static sptr<MyBox> alloc() { return new MyBox(); }

  MyBox(){};
};

TEST(NcObject, rcAndCast) {
  auto box = MyBox::alloc();
  EXPECT_EQ(box->retainCount(), 1);

  sptr<MyBox> box2(box); // copy constructor
  EXPECT_EQ(box->retainCount(), 2);

  sptr<MyBox> box3;
  box3 = box2;  // == operator
  EXPECT_EQ(box->retainCount(), 3);

  // cast to base
  sptr<NcObject> base(box);  // copy constructor
  EXPECT_EQ(box->retainCount(), 4);

  sptr<NcObject> base2;
  base2 = box;  // == operator
  EXPECT_EQ(box->retainCount(), 5);

  // cast to derived
  auto derived = static_pointer_cast<MyBox>(base);
  EXPECT_EQ(derived->retainCount(), 5);
  EXPECT_TRUE(derived.get() == box.get());
}
