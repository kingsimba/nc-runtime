#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_stdlib.h"

TEST(Stdlib, stackOrHeapAllocator)
{
    u8* stack = (u8*)nc_alloca(1024);
    StackOrHeapAllocator allocator(stack, 1024);
    EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 0u);
    EXPECT_EQ(allocator.allocArray<u8>(512) - stack, 512u);
    EXPECT_GT((size_t)(allocator.allocArray<u8>(1) - stack), 4096u); // stack used up, so it's on heap

    for (size_t i = 0; i < 100; i++)
    {
        EXPECT_GT((size_t)(allocator.allocArray<u8>(40960) - stack), 4096u);
    }

    EXPECT_EQ(allocator._moreHeapPointerCount(), 93);
}

TEST(Stdlib, TimeTick)
{
    TimeTick start = TimeTick::now();
    Thread::sleep(10);
    TimeTick duration = TimeTick::now() - start;
    EXPECT_GE(duration.ms(), 10);
}

TEST(Stdlib, someInt)
{
    auto v = Some<int>(5);
    EXPECT_TRUE(v.hasValue());
    EXPECT_EQ(v.value(), 5);
    EXPECT_EQ(v.value(), 5);
    EXPECT_EQ(v.Or(10), 5);

    v = noValue;
    EXPECT_FALSE(v.hasValue());
    EXPECT_EQ(v.Or(10), 10);
}

TEST(Stdlib, someString)
{
    auto s = Some<const char*>("hello");
    EXPECT_TRUE(s.hasValue());
    EXPECT_STREQ(s.value(), "hello");
    EXPECT_STREQ(s.Or("simba"), "hello");

    s = noValue;
    EXPECT_FALSE(s.hasValue());
    EXPECT_EQ(s.Or("simba"), "simba");
}

class CopyCountObject
{
public:
    forceinline CopyCountObject() : m_copyCount(0), m_moveCount(0) {}
    forceinline CopyCountObject(const CopyCountObject& r) : m_copyCount(r.m_copyCount + 1), m_moveCount(r.m_moveCount)
    {
    }
    forceinline CopyCountObject(const CopyCountObject&& r) noexcept
        : m_copyCount(r.m_copyCount), m_moveCount(r.m_moveCount + 1)
    {
    }

    int m_copyCount;
    int m_moveCount;
};

TEST(Stdlib, someCopyObject)
{
    auto getObject = []() -> Some<CopyCountObject> {
        auto rtn = Some<CopyCountObject>(CopyCountObject());
        return rtn;
    };
    auto s = getObject();
    EXPECT_TRUE(s.hasValue());
    EXPECT_EQ(s.value().m_copyCount, 0) << "There should be no copy. Only move.";
    EXPECT_LE(s.value().m_moveCount, 2);
    EXPECT_GE(s.value().m_moveCount, 1);
}

TEST(Stdlib, radianToDegree)
{
    EXPECT_NEAR(nc_radianToDegree(3.1415926f), 180, 0.0001f);
    EXPECT_NEAR(nc_degreeToRadian(-360), -3.1415926f * 2, 0.0001f);
}

TEST(Stdlib, isnan)
{
    EXPECT_TRUE(nc_isnan(NAN));
    EXPECT_FALSE(nc_isnan(3));
}

TEST(Stdlib, absMinMaxClampSwap)
{
    EXPECT_TRUE(nc_abs(-3.2) == 3.2);
    EXPECT_TRUE(nc_abs(3) == 3);

    EXPECT_TRUE(nc_min(3, 4) == 3);
    EXPECT_TRUE(nc_max(3, 4) == 4);
    EXPECT_TRUE(nc_clamp(1, 3, 5) == 3);
    EXPECT_TRUE(nc_clamp(4, 3, 5) == 4);
    EXPECT_TRUE(nc_clamp(5, 3, 5) == 5);
    EXPECT_TRUE(nc_clamp(6, 3, 5) == 5);

    EXPECT_EQ(nc_ntohl(0x01020304), 0x04030201u);
    EXPECT_EQ(nc_ntohs(0x0102), 0x0201u);

    u32 a = 0x01020304;
    u16 b = 0x0102;
    nc_flipU32(a);
    nc_flipU16(b);
    EXPECT_EQ(a, 0x04030201u);
    EXPECT_EQ(b, 0x0201u);
}

TEST(Stdlib, rect)
{
    Rect rct;
    rct.setAsNegativeMinimum();
    rct.combinePoint(5, 8);
    EXPECT_EQ(rct, Rect(5, 8, 5, 8));
    rct.combinePoint(6, -3);
    EXPECT_EQ(rct, Rect(5, -3, 6, 8));

    rct.combineRect(Rect(3, 9, 8, 12));
    EXPECT_EQ(rct, Rect(3, -3, 8, 12));

    rct.set(0, 0, 5, 5);
    EXPECT_FALSE(rct.isIntersectWith(Rect(4, 5, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(Rect(5, 4, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(Rect(5, 5, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(Rect(4, 4, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(Rect(4, 0, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(Rect(0, 4, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(Rect(-10, -10, 0, 0)));
    EXPECT_FALSE(rct.isIntersectWith(Rect(-10, -10, 1, 0)));
    EXPECT_TRUE(rct.isIntersectWith(Rect(-10, -10, 1, 1)));
}

TEST(Stdlib, rectF)
{
    RectF rct;
    rct.setAsNegativeMinimum();
    rct.combinePoint(5, 8);
    EXPECT_EQ(rct, RectF(5, 8, 5, 8));
    rct.combinePoint(6, -3);
    EXPECT_EQ(rct, RectF(5, -3, 6, 8));

    rct.combineRect(RectF(3, 9, 8, 12));
    EXPECT_EQ(rct, RectF(3, -3, 8, 12));

    rct.set(0, 0, 5, 5);
    EXPECT_FALSE(rct.isIntersectWith(RectF(4, 5, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(RectF(5, 4, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(RectF(5, 5, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(RectF(4, 4, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(RectF(4, 0, 10, 10)));
    EXPECT_TRUE(rct.isIntersectWith(RectF(0, 4, 10, 10)));
    EXPECT_FALSE(rct.isIntersectWith(RectF(-10, -10, 0, 0)));
    EXPECT_FALSE(rct.isIntersectWith(RectF(-10, -10, 1, 0)));
    EXPECT_TRUE(rct.isIntersectWith(RectF(-10, -10, 1, 1)));
}

TEST(Stdlib, rectFSetAsNegativeMinimum)
{
    RectF area;
    area.setAsNegativeMinimum();
    area.combinePoint(-1.0f, -2.0f);

    EXPECT_EQ(area.left, -1.0f);
    EXPECT_EQ(area.right, -1.0f);
    EXPECT_EQ(area.top, -2.0f);
    EXPECT_EQ(area.bottom, -2.0f);
}

namespace
{
static std::vector<const char*> g_stateLogs;
class IdleState : public MachineState
{
public:
    static sp<IdleState> alloc() { return NcObject::alloc<IdleState>(); }

    void stateBegin() override { g_stateLogs.push_back("IdleState begin"); }
    void stateUpdate() override { g_stateLogs.push_back("IdleState update"); }
    void stateEnd() override { g_stateLogs.push_back("IdleState end"); }
};

class DownloadState : public MachineState
{
public:
    static sp<DownloadState> alloc() { return NcObject::alloc<DownloadState>(); }

    void stateBegin() override { g_stateLogs.push_back("DownloadState begin"); }
    void stateUpdate() override { g_stateLogs.push_back("DownloadState update"); }
    void stateEnd() override { g_stateLogs.push_back("DownloadState end"); }
};
} // namespace

TEST(Stdlib, stateMachine)
{
    {
        StateMachine o;
        auto idleState = IdleState::alloc();
        auto downloadState = DownloadState::alloc();

        o.gotoState(idleState);
        o.spinOnce();
        o.gotoState(downloadState);
        o.spinOnce();
    }

    ASSERT_EQ(g_stateLogs.size(), 6);
    EXPECT_STREQ(g_stateLogs[0], "IdleState begin");
    EXPECT_STREQ(g_stateLogs[1], "IdleState update");
    EXPECT_STREQ(g_stateLogs[2], "IdleState end");
    EXPECT_STREQ(g_stateLogs[3], "DownloadState begin");
    EXPECT_STREQ(g_stateLogs[4], "DownloadState update");
    EXPECT_STREQ(g_stateLogs[5], "DownloadState end");
}
