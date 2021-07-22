#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_math.h"

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
