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
    EXPECT_STREQ(s->cstr(), "hello");
    EXPECT_EQ(s->length(), 5);

    // must call .get() to convert to ordinary pointer.
    EXPECT_EQ(_calculateStringLength(s), 5);

    s = NcString::allocWithBytes("cat litter", 10);
    EXPECT_STREQ(s->cstr(), "cat litter");
    EXPECT_EQ(s->length(), 10);
}

TEST(StringSlice, compare)
{
    EXPECT_TRUE("blood"_str->equals("blood"_str));
    EXPECT_FALSE("blood"_str->equals("bloom"_str));

    EXPECT_TRUE("blood"_str->equals("blood"));
    EXPECT_FALSE("blood"_str->equals("bloom"));
    EXPECT_TRUE("blood"_str->equals("blood"_s));
    EXPECT_FALSE("blood"_str->equals("bloom"_s));

    EXPECT_TRUE("blood"_str->equalsCaseInsensitive("BLOOD"));
    EXPECT_FALSE("blood"_str->equalsCaseInsensitive("BLOOM"));
    EXPECT_TRUE("blood"_str->equalsCaseInsensitive("BLOOD"_s));
    EXPECT_FALSE("blood"_str->equalsCaseInsensitive("BLOOM"_s));
}

TEST(NcString, join)
{
    sp<NcString> s = NcString::allocByJoiningSlices(vector<StringSlice>{"hello", "world"}, "---");
    EXPECT_STREQ(s->cstr(), "hello---world");
    EXPECT_EQ(s->length(), 13);

    auto strs = NcArray<NcString>::alloc();
    strs->addObject("hello"_str);
    strs->addObject("world"_str);
    s = NcString::allocByJoiningStrings(strs, "---");
    EXPECT_STREQ(s->cstr(), "hello---world");
    EXPECT_EQ(s->length(), 13);

    EXPECT_STREQ("==="_str->join(strs)->cstr(), "hello===world");
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
    EXPECT_STREQ(s->cstr(), "hello world");
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
    EXPECT_STREQ(NcString::format("%s shall come", "The Day")->cstr(), "The Day shall come");
    EXPECT_STREQ(NcString::format("%d is small but %lld is large", 123, 12345678901234ll)->cstr(),
                 "123 is small but 12345678901234 is large");
}
