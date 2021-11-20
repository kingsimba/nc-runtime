#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_string.h"
#include "nc_runtime/nc_array.h"

static int _calculateStringLength(NcString* str)
{
    return str->length();
}

TEST(NcString, basic)
{
    sp<NcString> s = NcString::allocWithCString("hello");
    EXPECT_EQ(s, "hello"_s);
    EXPECT_EQ(s->length(), 5);

    // must call .get() to convert to dumb pointer.
    EXPECT_EQ(_calculateStringLength(s.get()), 5);

    s = NcString::allocWithBytes("cat litter", 10);
    EXPECT_EQ(s, "cat litter"_s);
    EXPECT_EQ(s->length(), 10);
}

TEST(NcString, compare)
{
    EXPECT_TRUE("blood"_str == "blood"_str);
    EXPECT_TRUE("blood"_str != "bloom"_str);

    EXPECT_TRUE("blood"_str == "blood");
    EXPECT_TRUE("blood"_str != "bloom");
    EXPECT_TRUE("blood" == "blood"_str);
    EXPECT_TRUE("bloom" != "blood"_str);
    EXPECT_TRUE("blood"_s == "blood"_str);
    EXPECT_TRUE("bloom"_s != "blood"_str);

    EXPECT_TRUE("blood"_str->equalsCaseInsensitive("BLOOD"));
    EXPECT_FALSE("blood"_str->equalsCaseInsensitive("BLOOM"));
    EXPECT_TRUE("blood"_str->equalsCaseInsensitive("BLOOD"_s));
    EXPECT_FALSE("blood"_str->equalsCaseInsensitive("BLOOM"_s));
}

TEST(NcString, join)
{
    sp<NcString> s = NcString::allocByJoiningSlices(vector<StringSlice>{"hello", "world"}, "---");
    EXPECT_EQ(s, "hello---world");
    EXPECT_EQ(s->length(), 13);

    auto strs = NcArray<NcString>::alloc();
    strs->addObject("hello"_str);
    strs->addObject("world"_str);
    s = NcString::allocByJoiningStrings(strs.get(), "---");
    EXPECT_EQ(s, "hello---world");
    EXPECT_EQ(s->length(), 13);

    EXPECT_EQ("==="_str->join(strs.get()), "hello===world");
}

TEST(NcString, toSlice)
{
    sp<NcString> s = NcString::allocWithCString("hello world");
    auto slice = s->toSlice();
    EXPECT_EQ(s->retainCount(), 2); // slice holds a reference to the string
}

TEST(NcString, split)
{
    vector<StringSlice> slices = NcString::allocWithCString("hello---world")->split("---");
    auto s = NcString::allocByJoiningSlices(slices, " ");
    EXPECT_EQ(s, "hello world");
    EXPECT_EQ(s->length(), 11);
}

TEST(NcString, subslice)
{
    auto str = NcString::allocWithCString("hello---world");
    auto slice = str->subslice(5, 2);
    EXPECT_EQ(str->retainCount(), 2);
    EXPECT_TRUE(slice.equals("--"));
}

TEST(NcString, format)
{
    EXPECT_EQ(NcString::format("%s shall come", "The Day"), "The Day shall come");
    EXPECT_EQ(NcString::format("%d is small but %lld is large", 123, 12345678901234ll),
              "123 is small but 12345678901234 is large");
}
