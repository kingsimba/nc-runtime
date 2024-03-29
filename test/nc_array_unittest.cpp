#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_array.h"
#include "nc_runtime/nc_string.h"

using namespace std;

TEST(ArrayTest, basic)
{
    sp<NcString> perm;

    {
        auto str = NcString::allocWithCString("hello world");

        auto v = NcArray<NcString>::alloc();
        v->addObject(str);
        v->addObject(str);
        EXPECT_EQ(str->retainCount(), 3);

        auto pieces = str->split(" "_s);
        EXPECT_EQ(str->retainCount(), 5); // +2 because slices holds reference to the string
        ASSERT_EQ(pieces.size(), 2);
        for (auto s : pieces)
        {
            v->addObject(NcString::allocWithSlice(s));
        }

        EXPECT_EQ(v->firstObject().get(), str.get());
        EXPECT_EQ(v->lastObject().get(), v->objectAtIndex(v->size() - 1).get());

        EXPECT_EQ(v[0], "hello world");
        EXPECT_EQ(v[1], "hello world");
        EXPECT_EQ(v[2], "hello");
        EXPECT_EQ(v[3], "world");

        perm = v[0]; // +1
        EXPECT_EQ(perm->retainCount(), 6);

        // remove object
        v->removeObjectAtIndex(2);
        EXPECT_EQ(v->size(), 3);
        EXPECT_EQ(v[2], "world");
    }

    EXPECT_EQ(perm->retainCount(), 1);
}

TEST(ArrayTest, find)
{
    auto v = NcArray<NcString>::alloc();
    v->addObject(NcString::allocWithCString("hello"));
    v->addObject(NcString::allocWithCString("world"));

    auto startWord = "w"_s;
    auto obj = v->findWithCondition([&](NcString* v) { return v->startsWith(startWord); });

    ASSERT_TRUE(obj != nullptr);
    EXPECT_EQ(obj, "world");

    obj = v->findWithCondition([](NcString* v) { return v->startsWith("s"); });

    ASSERT_TRUE(obj == nullptr);

    EXPECT_EQ(v->indexOfObjectWithCondition([&](NcString* v) { return v->startsWith(startWord); }), 1);
    EXPECT_EQ(v->indexOfObjectWithCondition([&](NcString* v) { return v->startsWith("s"_s); }), -1);

    EXPECT_EQ(v->indexOfObject(NcString::allocWithCString("hello")), 0);
    EXPECT_EQ(v->indexOfObject(NcString::allocWithCString("non-exist")), -1);
}
