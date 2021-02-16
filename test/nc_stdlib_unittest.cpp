#include "stdafx_nc_runtime_test.h"
#include "nc_stdlib.h"

TEST(Stdlib, hash) {
  EXPECT_TRUE(Math_hashSizeT(1) != Math_hashSizeT(2));
#ifdef NC_ENVIRONMENT_64
  EXPECT_EQ(Math_hashU64(1), Math_hashSizeT(1));
#else
  EXPECT_EQ(Math_hashU32(1), Math_hashSizeT(1));
#endif
}