#include "stdafx_nc_runtime_test.h"
#include "string_slice.h"
#include "nc_string.h"

TEST(StringSlice, subslice) {
  auto s = StringSlice("To err is human, to forgive divine.");
  EXPECT_STREQ(s.subsliceFrom(-7).toString()->cstr(), "divine.");
  EXPECT_STREQ(s.subsliceFrom(3).toString()->cstr(), "err is human, to forgive divine.");
  EXPECT_STREQ(s.subslice(10, 5).toString()->cstr(), "human");
  EXPECT_STREQ(s.subsliceInRange(Range_make(10, 5)).toString()->cstr(), "human");
}

TEST(StringSlice, split) {
  auto s = StringSlice("You should name a 分隔符 variable with the same care 分隔符 as you are "
                       "naming your first born child.");
  auto pieces = s.split("分隔符 ");
  ASSERT_EQ(pieces.size(), 3);
  EXPECT_TRUE(pieces[0].equals("You should name a "));
  EXPECT_TRUE(pieces[1].equals("variable with the same care "));
  EXPECT_TRUE(pieces[2].equals("as you are naming your first born child."));

  // non-exist splitter should produce a single result
  pieces = s.split("non-exist");
  ASSERT_EQ(pieces.size(), 1);
  EXPECT_TRUE(pieces[0].length() == s.length());

  {
    StringSlice pieces[3];
    EXPECT_EQ(StringSlice("hello--world--hello--world").splitWithLimit("--", pieces, countof(pieces)), 3);
    EXPECT_TRUE(pieces[1].equals("world"));
  }
}

TEST(StringSlice, find) {
  auto s = "hello 世界"_s;
  EXPECT_EQ(s.find(U'世'), Range_make(6, 3));
  EXPECT_EQ(s.find(U'界'), Range_make(9, 3));

  EXPECT_EQ(s.find('l'), 2);
  EXPECT_EQ(s.rfind('l'), 3);
  EXPECT_EQ(s.find('t'), -1);
}

TEST(StringSlice, startsEndsWith) {
  auto s = "hello 世界"_s;

  EXPECT_TRUE(s.startsWith("hello "));
  EXPECT_TRUE(s.startsWith("hello 世界"));
  EXPECT_FALSE(s.startsWith("hello 世界1"));
  EXPECT_FALSE(s.startsWith("世界"));

  EXPECT_TRUE(s.endsWith("hello 世界"));
  EXPECT_TRUE(s.endsWith("世界"));
  EXPECT_FALSE(s.endsWith("1hello 世界"));
  EXPECT_FALSE(s.endsWith("hello"));
}

TEST(StringSlice, replace) {
  auto str = NcString::allocWithCString("folder1/folder2/file.png");
  auto pos = str->rfind('.');
  ASSERT_TRUE(pos != -1);
  EXPECT_STREQ(str->stringByReplacingStringInRange(Range_make(pos, str->length() - pos), ".jpg")->cstr(),
	       "folder1/folder2/file.jpg");
}
