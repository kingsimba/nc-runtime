#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/quaternion.h"

TEST(Quaternion, basic)
{
    Quaternion q;
    q.initWithRPY(1, 0, 0);
    RollPitchYaw angles = q.toRPY();
    EXPECT_NEAR(angles.roll, 1, 0.01f);
    EXPECT_NEAR(angles.pitch, 0, 0.01f);
    EXPECT_NEAR(angles.yaw, 0, 0.01f);

    q.initWithRPY(0, 1, 0);
    angles = q.toRPY();
    EXPECT_NEAR(angles.roll, 0, 0.01f);
    EXPECT_NEAR(angles.pitch, 1, 0.01f);
    EXPECT_NEAR(angles.yaw, 0, 0.01f);

    q.initWithRPY(0, 0, 1);
    angles = q.toRPY();
    EXPECT_NEAR(angles.roll, 0, 0.01f);
    EXPECT_NEAR(angles.pitch, 0, 0.01f);
    EXPECT_NEAR(angles.yaw, 1, 0.01f);

    q.initWithRPY(1, 1.2f, 1.3f);
    angles = q.toRPY();
    EXPECT_NEAR(angles.roll, 1, 0.01f);
    EXPECT_NEAR(angles.pitch, 1.2, 0.01f);
    EXPECT_NEAR(angles.yaw, 1.3, 0.01f);

    // rotate vector
    q.initWithRPY(0, 0, NC_PI / 2);
    Vector3 r = q.transformVector(Vector3(10, 0, 0));

    EXPECT_NEAR(r.x, 0, 0.0001);
    EXPECT_NEAR(r.y, 10, 0.0001);
    EXPECT_FLOAT_EQ(r.z, 0);
}

TEST(Quaternion, multiply)
{
    Quaternion q1, q2, q3;
    q1.initWithRPY(1, 0, 0);
    q2.initWithRPY(0, 1.2f, 0);
    q3.initWithRPY(0, 0, 1.3f);

    Quaternion combined;
    combined = Quaternion_multiply(q2, q1);
    combined = Quaternion_multiply(q3, combined);

    auto angles = combined.toRPY();
    EXPECT_NEAR(angles.roll, 1, 0.01f);
    EXPECT_NEAR(angles.pitch, 1.2f, 0.01f);
    EXPECT_NEAR(angles.yaw, 1.3f, 0.01f);

    q2.multiply(q3);
    q1.multiply(q2);
    angles = q1.toRPY();
    EXPECT_NEAR(angles.roll, 1, 0.01f);
    EXPECT_NEAR(angles.pitch, 1.2, 0.01f);
    EXPECT_NEAR(angles.yaw, 1.3f, 0.01f);
}
