#include "stdafx_nc_runtime_test.h"
#include "nc_object.h"

class MyBox : public NcObject {
public:
  static sptr<MyBox> alloc() { return new MyBox(); }

private:
  MyBox(){};
  ~MyBox(){};
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
  EXPECT_EQ(derived.use_count(), 5);
  EXPECT_TRUE(derived.get() == box.get());
  EXPECT_TRUE(derived == box);
}

TEST(NcObject, copyNULL) {
  sptr<MyBox> box;
  EXPECT_TRUE(box == NULL);

  sptr<MyBox> box2(box);
  EXPECT_TRUE(box2 == NULL);
}

TEST(NcObject, compare) {
  auto box = MyBox::alloc();
  EXPECT_EQ(box->retainCount(), 1);

  EXPECT_TRUE(box != NULL);

  box.reset();

  EXPECT_TRUE(box == NULL);
}

TEST(NcObject, retainReleaseSmartPointer) {
  // allow using retain() and release() on smart pointer
  auto box = MyBox::alloc();
  EXPECT_EQ(box->retainCount(), 1);

  EXPECT_TRUE(box != NULL);

  // retain. This is unnecessary. Still, we support it to prevent error.
  sptr<MyBox> box2 = retain<MyBox>(box);
  // release. Totally legal
  release(box);

  EXPECT_TRUE(box == NULL);
  EXPECT_TRUE(box2 != NULL);
  EXPECT_EQ(box2->retainCount(), 1);
}