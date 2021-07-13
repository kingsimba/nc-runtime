#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_math.h"

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
