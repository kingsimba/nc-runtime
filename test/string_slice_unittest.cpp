#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/string_slice.h"
#include "nc_runtime/nc_string.h"

TEST(StringSlice, compare)
{
    EXPECT_TRUE("blood"_s == "blood"_s);
    EXPECT_TRUE("blood"_s != "bloom"_s);

    EXPECT_TRUE("blood"_s == "blood");
    EXPECT_TRUE("blood"_s != "bloom");
    EXPECT_TRUE("blood" == "blood"_s);
    EXPECT_TRUE("bloom" != "blood"_s);

    EXPECT_TRUE("blood"_s.equalsCaseInsensitive("BLOOD"));
    EXPECT_FALSE("blood"_s.equalsCaseInsensitive("BLOOM"));
    EXPECT_TRUE("blood"_s.equalsCaseInsensitive("BLOOD"_s));
    EXPECT_FALSE("blood"_s.equalsCaseInsensitive("BLOOM"_s));
}

TEST(StringSlice, format)
{
    EXPECT_EQ(StringSlice::format("%s %s", "hello", "world"), "hello world");
}

TEST(StringSlice, iter)
{
    auto iter = "To err is human, to forgive divine."_s.iterForSplitting(" ");
    StringSlice slice;
    bool found = false;
    while (iter.next(&slice))
    {
        if (slice.equals("human,"))
            found = true;
    }

    EXPECT_TRUE(found);
}

TEST(StringSlice, tokenize)
{
    auto tokens = "Whoa! Fireworks!?"_s.tokenize(" ,?!");
    ASSERT_EQ(tokens.size(), 2);
    ASSERT_EQ(tokens[0], "Whoa");
    ASSERT_EQ(tokens[1], "Fireworks");
}

//
// TEST(StringSlice, splitPerformance) {
//  auto s = "Whoa! Fireworks!? Whoa! Fireworks!?Whoa! Fireworks!?Whoa! Fireworks!?Whoa! Fireworks!?Whoa!
//  Fireworks!?"_s; StringSlice slice; auto seps = "!?Whoa!"_s; for (int i = 0; i < 10000000; i++) {
//    auto iter = s.iterForSplitting(seps);
//    while (iter.next(&slice)) {
//
//    }
//  }
//}

TEST(StringSlice, subslice)
{
    auto s = StringSlice("To err is human, to forgive divine.");
    EXPECT_EQ(s.subsliceFrom(-7), "divine.");
    EXPECT_EQ(s.subsliceFrom(3), "err is human, to forgive divine.");
    EXPECT_EQ(s.subslice(10, 5), "human");
    EXPECT_EQ(s.subslice(Range(10, 5)), "human");

    // count subslice
    EXPECT_EQ("hello world hello world hello"_s.countSlice("hello"), 3);
    EXPECT_EQ("hello"_s.countSlice("hello"), 1);
    EXPECT_EQ("hello"_s.countSlice("hello world"), 0);
}

TEST(StringSlice, splitAndJoin)
{
    auto s = StringSlice("You should name a 分隔符 variable with the same care 分隔符 as you are "
                         "naming your first born child.");
    {
        auto pieces = s.split("分隔符 ");
        ASSERT_EQ(pieces.size(), 3);
        EXPECT_EQ(pieces[0], "You should name a ");
        EXPECT_EQ(pieces[1], "variable with the same care ");
        EXPECT_EQ(pieces[2], "as you are naming your first born child.");
    }

    {
        StringSlice pieces[3];
        EXPECT_EQ("hello--world--hello--world"_s.splitWithLimit("--", pieces, countof(pieces)), 3);
        EXPECT_EQ(pieces[0], "hello");
        EXPECT_EQ(pieces[1], "world");
        EXPECT_EQ(pieces[0], "hello");

        EXPECT_EQ("___"_s.join(pieces, 3), "hello___world___hello");
    }

    {
        std::vector<StringSlice> lines({"hello", "world", ""});
        EXPECT_EQ("\n"_s.join(lines), "hello\nworld\n");
    }
}

TEST(StringSlice, splitBug1)
{
    auto s = "Fireworks-------Fireworks-------Fireworks-------Fireworks--"_s;
    StringSlice slice;
    auto slices = s.split("-------"_s);
    ASSERT_EQ(slices.size(), 4);
    EXPECT_EQ(slices.back(), "Fireworks--");
}

TEST(StringSlice, splitNonexistSep)
{
    // non-exist splitter should produce a single result
    auto s = "hello world"_s;
    auto pieces = s.split("non-exist");
    ASSERT_EQ(pieces.size(), 1);
    EXPECT_EQ(pieces[0], s);
}

TEST(StringSlice, splitShouldKeepEmptySlices)
{
    // 3 separators should create 4 slices
    auto s = ",,,"_s;
    StringSlice slice;
    auto slices = s.split(","_s);
    ASSERT_EQ(slices.size(), 4);
    EXPECT_EQ(slices.back(), "");
}

TEST(StringSlice, find)
{
    auto s = "hello 世界"_s;
    EXPECT_EQ(s.find(U'世'), Range(6, 3));
    EXPECT_EQ(s.find(U'界'), Range(9, 3));

    EXPECT_EQ(s.find('l'), 2);
    EXPECT_EQ(s.rfind('l'), 3);
    EXPECT_EQ(s.find('t'), -1);
}

TEST(StringSlice, startsEndsWith)
{
    auto s = "hello 世界"_s;

    // using _s is faster. Because the compiler will determine the length. Not through strlen()
    EXPECT_TRUE(s.startsWith("hello "_s));
    EXPECT_TRUE(s.startsWith("hello 世界"_s));
    EXPECT_FALSE(s.startsWith("hello 世界1"_s));
    EXPECT_FALSE(s.startsWith("世界"_s));

    EXPECT_TRUE(s.endsWith("hello 世界"_s));
    EXPECT_TRUE(s.endsWith("世界"_s));
    EXPECT_FALSE(s.endsWith("1hello 世界"_s));
    EXPECT_FALSE(s.endsWith("hello"_s));
}

TEST(StringSlice, replace)
{
    auto str = "folder1/folder2/file.png"_s;
    auto pos = str.rfind('.');
    ASSERT_TRUE(pos != -1);
    EXPECT_EQ(str.replaceInRange(Range(pos, str.length() - pos), ".jpg"), "folder1/folder2/file.jpg");
}
