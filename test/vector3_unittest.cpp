#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/vector3.h"
#include "nc_runtime/vector3i.h"

TEST(Vector3, basic)
{
    Vector3 v1(1, 2, 3);
    Vector3 v2(2, 3, 4);

    EXPECT_EQ(v1 + v2, Vector3(3, 5, 7));
    EXPECT_EQ(v1 - v2, Vector3(-1, -1, -1));

    EXPECT_EQ(v1 * 2, Vector3(2, 4, 6));
    EXPECT_EQ(v1 / 2, Vector3(0.5f, 1.0f, 1.5f));

    EXPECT_EQ(Vector3(3, 4, 5).length(), sqrtf(50));
    EXPECT_EQ(Vector3(3, 4, 5).lengthSquared(), 50);
}

TEST(Vector3i, basic)
{
    Vector3i v1(1, 2, 3);
    Vector3i v2(2, 3, 4);

    EXPECT_EQ(v1 + v2, Vector3i(3, 5, 7));
    EXPECT_EQ(v1 - v2, Vector3i(-1, -1, -1));

    EXPECT_EQ(v1 * 2, Vector3i(2, 4, 6));
    EXPECT_EQ(v1 / 2, Vector3i((int)0.5f, (int)1.0f, (int)1.5f));

    EXPECT_EQ(Vector3i(3, 4, 5).length(), (int)sqrtf(50));
    EXPECT_EQ(Vector3i(3, 4, 5).lengthSquared(), 50);
}
