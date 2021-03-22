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
  EXPECT_GT((size_t)(allocator.allocArray<u8>(1) - stack), 4096u);  // stack used up, so it's on heap

  for (size_t i = 0; i < 100; i++) {
    EXPECT_GT((size_t)(allocator.allocArray<u8>(40960) - stack), 4096u);
  }

  EXPECT_EQ(allocator._moreHeapPointerCount(), 93);
}

TEST(Stdlib, TimeTick) {
  TimeTick start = TimeTick::now();
  Thread::sleep(10);
  TimeTick duration = TimeTick::now() - start;
  EXPECT_GE(duration.ms(), 10);
}

TEST(Stdlib, ManualResetEvent) {
  ManualResetEvent e;

  std::thread t([&] {
    Thread::sleep(100);
    e.set();
  });

  // must wait until reset
  EXPECT_GE(TimeTick::measure([&] { e.wait(); }).ms(), 100);
  t.join();

  // not reset(). So wait on it will not block
  EXPECT_LT(TimeTick::measure([&] { e.wait(); }).ms(), 1);

  // reset() again. Wait will block
  e.reset();
  EXPECT_GE(TimeTick::measure([&] { EXPECT_FALSE(e.waitWithTimeout(10)); }).ms(), 10);
}

TEST(Stdlib, MutexGuard) {
  int counter = 0;
  int counterUnprotected = 0;
  const static int REPEAT = 10000;
  recursive_mutex arrMutex;
  thread t1([&] {
    for (int i = 0; i < REPEAT; i++) {
      synchronized(arr) { counter++; }
    }
  });

  thread t2([&] {
    for (int i = 0; i < REPEAT; i++) {
      synchronized(arr) { counter++; }
      counterUnprotected++;
    }
  });

  t1.join();
  t2.join();

  EXPECT_EQ(counter, REPEAT * 2);
  EXPECT_NE(counterUnprotected, REPEAT * 2);
}
