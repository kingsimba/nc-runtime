#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/vector3.h"

TEST(Vector3, basic)
{
    Vector3 v1 = vec3(1, 2, 3);
    Vector3 v2 = vec3(2, 3, 4);

    EXPECT_EQ(v1 + v2, vec3(3, 5, 7));
    EXPECT_EQ(v1 - v2, vec3(-1, -1, -1));

    EXPECT_EQ(v1 * 2, vec3(2, 4, 6));
    EXPECT_EQ(v1 / 2, vec3(0.5f, 1.0f, 1.5f));

    EXPECT_EQ(vec3(3, 4, 5).length(), sqrtf(50));
    EXPECT_EQ(vec3(3, 4, 5).lengthSquared(), 50);
}
