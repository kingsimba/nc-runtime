#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/mutex.h"

class MutexTest : public ::testing::Test
{
public:
    Mutex m_mutex;
    int m_counter GUARDED_BY(m_mutex);
    int m_counterUnprotected;
};

TEST_F(MutexTest, basic)
{
    const static int REPEAT = 5000000;
    {
        LockGuard lg(m_mutex);
        m_counter = 0;
    }
    m_counterUnprotected = 0;

    thread t1([&] {
        for (int i = 0; i < REPEAT; i++)
        {
            {
                LockGuard lg(m_mutex);
                m_counter++;
            }
            m_counterUnprotected++;
        }
    });

    thread t2([&] {
        for (int i = 0; i < REPEAT; i++)
        {
            {
                LockGuard lg(m_mutex);
                m_counter++;
            }
            m_counterUnprotected++;
        }
    });

    t1.join();
    t2.join();

    {
        LockGuard lg(m_mutex);
        EXPECT_EQ(m_counter, REPEAT * 2);
    }
    EXPECT_LT(m_counterUnprotected, REPEAT * 2);
}
