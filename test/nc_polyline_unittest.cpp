#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_polyline.h"

TEST(NcPolylineTest, polyline)
{
    std::vector<nc::Vector2> points;
    points.push_back({0, 0});
    points.push_back({1, 2});
    points.push_back({3, 4});

    sp<NcPolyline> polyline = NcPolyline::alloc(points.data(), (int)points.size(), false);
    EXPECT_EQ(polyline->points()[0], vec2(0, 0));
    EXPECT_EQ(polyline->points()[1], vec2(1, 2));
    EXPECT_EQ(polyline->points()[2], vec2(3, 4));
    EXPECT_EQ(polyline->pointCount(), 3);
    EXPECT_FALSE(polyline->isClosed());
}

TEST(NcPolylineTest, polygon)
{
    std::vector<nc::Vector2> points;
    points.push_back({0, 0});
    points.push_back({1, 0});
    points.push_back({0, 2});

    sp<NcPolyline> ploygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon->pointCount(), 3);
    EXPECT_TRUE(ploygon->isClosed());
}
