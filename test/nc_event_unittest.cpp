#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_event.h"

TEST(EventTest, manualReset)
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

TEST(EventTest, autoReset)
{
    ResetableEvent e(false, true);
    ResetableEvent feedbackEvent(false, true);

    bool t1Signaled = false;
    bool t2Signaled = false;

    std::thread t1([&] {
        e.wait();
        t1Signaled = true;
        feedbackEvent.set();
    });
    std::thread t2([&] {
        e.wait();
        t2Signaled = true;
        feedbackEvent.set();
    });

    e.set();
    feedbackEvent.wait();
    Thread::sleep(100); // Must wait, in case two thread returns together because of bug.
    EXPECT_EQ(t1Signaled + t2Signaled, 1) << "only one thread returns";
    e.set();
    feedbackEvent.wait();
    Thread::sleep(100);
    EXPECT_EQ(t1Signaled + t2Signaled, 2) << "two threads returns";
    t1.join();
    t2.join();
}
