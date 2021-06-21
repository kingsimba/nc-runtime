#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_object.h"

class MyBox : public NcObject
{
  public:
    static sp<MyBox> alloc() { return new MyBox(); }

  private:
    MyBox() { m_data = 3; };
    ~MyBox(){};

  private:
    int m_data;
};

class MyString : public NcObject
{
  public:
    static sp<MyString> alloc() { return new MyString(); }

  private:
    MyString(){};
    ~MyString(){};
};

TEST(NcObject, weak)
{
    auto box = MyBox::alloc();
    EXPECT_EQ(box->retainCount(), 1);
    EXPECT_EQ(box->weakCount(), 0);

    // test constructor
    wp<MyBox> w(box);
    EXPECT_EQ(box->retainCount(), 1);
    EXPECT_EQ(box->weakCount(), 1);

    // copy weak
    wp<MyBox> w2 = w;
    EXPECT_EQ(box->retainCount(), 1);
    EXPECT_EQ(box->weakCount(), 2);

    // reset weak
    w2.reset();
    EXPECT_EQ(box->weakCount(), 1);

    // weak -> strong
    auto box2 = w.lock();
    EXPECT_EQ(box->retainCount(), 2);
    EXPECT_EQ(box->weakCount(), 1);

    // reset strong
    box.reset();
    EXPECT_EQ(box2->retainCount(), 1);
    EXPECT_EQ(box2->weakCount(), 1);

    // test assign operator
    w = box2;
    w = box2.get();
    EXPECT_EQ(box2->retainCount(), 1);
    EXPECT_EQ(box2->weakCount(), 1);

    // reset strong, weak will expire
    box2.reset();
    box = w.lock();
    EXPECT_TRUE(box == NULL);
}

TEST(NcObject, rcAndCast)
{
    auto box = MyBox::alloc();
    EXPECT_EQ(box->retainCount(), 1);

    sp<MyBox> box2(box); // copy constructor
    EXPECT_EQ(box->retainCount(), 2);

    sp<MyBox> box3;
    box3 = box2; // == operator
    EXPECT_EQ(box->retainCount(), 3);

    // cast to base
    sp<NcObject> base(box); // copy constructor
    EXPECT_EQ(box->retainCount(), 4);

    sp<NcObject> base2;
    base2 = box; // == operator
    EXPECT_EQ(box->retainCount(), 5);

    // cast to derived
    auto derived = static_pointer_cast<MyBox>(base);
    EXPECT_EQ(derived.use_count(), 6);
    EXPECT_TRUE(derived.get() == box.get());
    EXPECT_TRUE(derived == box);
}

TEST(NcObject, copyNULL)
{
    sp<MyBox> box;
    EXPECT_TRUE(box == NULL);

    sp<MyBox> box2(box);
    EXPECT_TRUE(box2 == NULL);
}

TEST(NcObject, compare)
{
    auto box = MyBox::alloc();
    EXPECT_EQ(box->retainCount(), 1);

    EXPECT_TRUE(box != NULL);

    box.reset();

    EXPECT_TRUE(box == NULL);
}

TEST(NcObject, retainReleaseSmartPointer)
{
    // allow using retain() and release() on smart pointer
    auto box = MyBox::alloc();
    EXPECT_EQ(box->retainCount(), 1);

    EXPECT_TRUE(box != NULL);

    // retain. This is unnecessary(should use box2 = box).
    // But still, we support it to prevent error.
    sp<MyBox> box2 = retain<MyBox>(box);

    // ERROR: This will not increase RC. So it's an error.
    // sp<MyBox> box2 = box.get();

    box.reset();

    EXPECT_TRUE(box == NULL);
    EXPECT_TRUE(box2 != NULL);
    EXPECT_EQ(box2->retainCount(), 1);
}

TEST(NcObject, isKindOf)
{
    auto box = MyBox::alloc();
    EXPECT_EQ(box->retainCount(), 1);
    EXPECT_TRUE(box->isKindOf<MyBox>());
    EXPECT_TRUE(box->isKindOf<NcObject>());
    EXPECT_FALSE(box->isKindOf<MyString>());
}

TEST(NcObject, tryStd)
{
    auto s = std::make_shared<int>(3);
    std::weak_ptr<int> w(s);
    s.reset();
    w.reset();
}
