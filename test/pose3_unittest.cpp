#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/pose2.h"
#include "nc_runtime/pose3.h"

TEST(PoseTest, pose3)
{
    Pose3 pose;

    // init
    EXPECT_EQ(pose.pos, Vector3::zero());
    EXPECT_EQ(pose.ori, Quaternion::identity());

    // copy
    pose = Pose3(Vector3(1, 2, 3), Quaternion::makeWithRPY(0.1f, 0.2f, 0.3f));

    // equals
    EXPECT_TRUE(pose == Pose3(Vector3(1, 2, 3), Quaternion::makeWithRPY(0.1f, 0.2f, 0.3f)));
    EXPECT_TRUE(pose != Pose3(Vector3(2, 2, 3), Quaternion::makeWithRPY(0.1f, 0.2f, 0.3f)));

    // convert with pose2
    Pose2 pose2 = pose.toPose2();
    EXPECT_EQ(pose2.pos, Vector2(1, 2));
    EXPECT_NEAR(pose2.ori, 0.3, 0.0001);
    pose = Pose3::makeWithPose2(pose2);
    EXPECT_EQ(pose.pos, Vector3(1, 2, 0));
    EXPECT_EQ(pose.ori.toRPY().roll, 0);
    EXPECT_EQ(pose.ori.toRPY().pitch, 0);
    EXPECT_NEAR(pose.ori.toRPY().yaw, 0.3, 0.0001);
}