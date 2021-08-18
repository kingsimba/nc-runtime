#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_event.h"

TEST(EventTest, resetableEvent)
{
    ResetableEvent e;

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
