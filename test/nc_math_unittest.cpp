#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_math.h"

using namespace nc;
using namespace std;

TEST(Math, hash)
{
    EXPECT_TRUE(Math_hashSizeT(1) != Math_hashSizeT(2));
#ifdef NC_ENVIRONMENT_64
    EXPECT_EQ(Math_hashU64(1), Math_hashSizeT(1));
#else
    EXPECT_EQ(Math_hashU32(1), Math_hashSizeT(1));
#endif
}

TEST(Math, wrapTwoPi)
{
    EXPECT_NEAR(Math_wrapTwoPi(NC_TWO_PI * 4 + 0.1f), 0.1f, 0.00001f);
    EXPECT_NEAR(Math_wrapTwoPi(-NC_TWO_PI * 4 + 0.1f), 0.1f, 0.00001f);
}

TEST(Math, turnAngle)
{
    EXPECT_NEAR(Math_turnAngle(-0.1f, 0.1f), 0.2f, 0.00001f);
    EXPECT_NEAR(Math_turnAngle(0.1f, -0.1f), -0.2f, 0.00001f);

    EXPECT_NEAR(Math_turnAngle(NC_TWO_PI - 0.1f, 0.1f), 0.2f, 0.00001f);
    EXPECT_NEAR(Math_turnAngle(0.1f, NC_TWO_PI - 0.1f), -0.2f, 0.00001f);
}

TEST(Math, Math_pointInPolygon)
{
    // 0 point
    {
        vector<Vector2> points;
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
    }

    // 1 point
    {
        vector<Vector2> points{{0, 0}};
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
    }

    // 2 points
    {
        vector<Vector2> points{{0, 0}, {10, 0}};
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(5, 0)));
    }

    // more points
    {
        vector<Vector2> points{{0, 0}, {10, 0}, {10, 10}, {0, 10}};
        EXPECT_TRUE(Math_pointInPolygon(points.data(), points.size(), Vector2(5, 5)));
        EXPECT_TRUE(Math_pointInPolygon(points.data(), points.size(), Vector2(10, 5)));
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(10.01f, 5)));
    }
}
