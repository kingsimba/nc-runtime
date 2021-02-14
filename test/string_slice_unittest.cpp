#include "stdafx_nc_runtime_test.h"
#include "string_slice.h"

TEST(StringSlice, split) {
  auto s = StringSlice("You should name a 分隔符 variable with the same care 分隔符 as you are "
                       "naming your first born child.");
  auto pieces = s.split("分隔符 ");
  EXPECT_TRUE(pieces[0].equals("You should name a "));
  EXPECT_TRUE(pieces[1].equals("variable with the same care "));
  EXPECT_TRUE(pieces[2].equals("as you are naming your first born child."));
}
