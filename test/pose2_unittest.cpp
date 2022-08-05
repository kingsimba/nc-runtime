#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/pose2.h"

TEST(Pose2Test, basic)
{
    Pose2 pose;

    // init
    EXPECT_EQ(pose.pos, Vector2::zero());
    EXPECT_EQ(pose.ori, 0);

    // copy
    pose = Pose2(Vector2(1, 2), 0.3);

    // equals
    EXPECT_TRUE(pose == Pose2(Vector2(1, 2), 0.3));
    EXPECT_TRUE(pose != Pose2(Vector2(2, 2), 0.3));
}

TEST(Pose2Test, inverse)
{
    Pose2 pose(Vector2(1, 2), 0.3);
    pose = pose.inverse();
    EXPECT_EQ(pose.pos, Vector2(-1, -2));
    EXPECT_NEAR(pose.ori, -0.3, 0.0001);
}

TEST(Pose2Test, concatenate)
{
    Pose2 pose(Vector2(0, 1), NC_PI_4);
    Pose2 pose2(Vector2(sqrt(2) / 2, sqrt(2) / 2), NC_PI_4);

    EXPECT_NEAR(pose2.pos.length(), 1, 0.0001);

    //
    {
        Pose2 finalPose = pose2 * pose;

        EXPECT_NEAR(finalPose.ori, NC_PI_2, 0.0001);

        EXPECT_NEAR(finalPose.pos.x, 0, 0.0001);
        EXPECT_NEAR(finalPose.pos.y, 1.414, 0.001);
    }

    // inverse sequence
    {
        Pose2 finalPose2 = pose * pose2;

        EXPECT_NEAR(finalPose2.ori, NC_PI_2, 0.0001);

        EXPECT_NEAR(finalPose2.pos.x, 0, 0.0001);
        EXPECT_NEAR(finalPose2.pos.y, 2, 0.001);
    }

    // another
    {
        Pose2 finalPose2 = pose * pose2.inverse();

        EXPECT_NEAR(finalPose2.ori, 0, 0.0001);

        EXPECT_NEAR(finalPose2.pos.x, 0, 0.0001);
        EXPECT_NEAR(finalPose2.pos.y, 0, 0.001);
    }
}