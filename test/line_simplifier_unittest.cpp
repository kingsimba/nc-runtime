#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/line_simplifier.h"

TEST(LineSimplifierTest, basic)
{
    std::vector<Vector2> points0 = {{0, 0}, {2, 12}, {4, 16}, {8, 10}, {8, 0}};

    std::vector<Vector2> points;
    LineSimplifier ls;
    int count;

    points = points0;
    count = ls.douglasPeucker(points.data(), (int)points.size(), 10.0f, INT_MAX);
    ASSERT_EQ(count, 3);
    EXPECT_EQ(points[0], Vector2(0, 0));
    EXPECT_EQ(points[1], Vector2(4, 16));
    EXPECT_EQ(points[2], Vector2(8, 0));

    points = points0;
    count = ls.douglasPeucker(points.data(), (int)points.size(), 0.5f, INT_MAX);
    ASSERT_EQ(count, 5);
    EXPECT_EQ(points[0], Vector2(0, 0));
    EXPECT_EQ(points[1], Vector2(2, 12));
    EXPECT_EQ(points[2], Vector2(4, 16));
    EXPECT_EQ(points[3], Vector2(8, 10));
    EXPECT_EQ(points[4], Vector2(8, 0));

    points = points0;
    count = ls.douglasPeucker(points.data(), (int)points.size(), 0.5f, 4);
    ASSERT_EQ(count, 4);
    EXPECT_EQ(points[0], Vector2(0, 0));
    EXPECT_EQ(points[1], Vector2(4, 16));
    EXPECT_EQ(points[2], Vector2(8, 10));
    EXPECT_EQ(points[3], Vector2(8, 0));
}

TEST(LineSimplifierTest, emptyInputProducesEmptyOutput)
{
    LineSimplifier ls;
    int count = ls.douglasPeucker(nullptr, 0, 0.02f, 10);
    EXPECT_EQ(count, 0);
}

TEST(LineSimplifierTest, nearStraightCase)
{
    std::vector<Vector2> points0 = {{6.775138f, 14.90059f},  {6.750138f, 14.900599f}, {6.725138f, 14.900599f},
                                    {6.700138f, 14.900599f}, {6.675138f, 14.900599f}, {6.650138f, 14.900599f},
                                    {6.625138f, 14.900599f}, {6.600138f, 14.900599f}, {6.575138f, 14.900599f},
                                    {6.550138f, 14.900599f}, {6.525138f, 14.900599f}, {6.500138f, 14.900599f}};

    std::vector<Vector2> points;
    LineSimplifier ls;
    int count;

    points = points0;
    count = ls.douglasPeucker(points.data(), (int)points.size(), 0.01f, INT_MAX);
    ASSERT_EQ(count, 2);
    EXPECT_EQ(points[0], points0.front());
    EXPECT_EQ(points[1], points0.back());
}
