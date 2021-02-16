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

TEST(Stdlib, stackOrHeapAllocator) {
  u8* stack = (u8*)alloca(1024);
  StackOrHeapAllocator allocator(stack, 1024);
  EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 0u);
  EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 512u);
  EXPECT_GT(allocator.allocArray<u8>(1) - stack, 4096u); // stack used up, so it's on heap

  for (size_t i = 0; i < 100; i++) {
    EXPECT_GT(allocator.allocArray<u8>(40960) - stack, 4096);
  }

  EXPECT_EQ(allocator._moreHeapPointerCount(), 93);
}
