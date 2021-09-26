#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_polyline.h"

TEST(NcPolylineTest, polyline)
{
    std::vector<nc::Vector2> points;
    points.push_back({0, 0});
    points.push_back({2, 0});
    points.push_back({0, 2});
    points.push_back({3, 3});
    points.push_back({2, 5});

    NcPolyline polyline(points.data(), (int)points.size(), false);
    EXPECT_EQ(polyline.pointCount(), 5);
    EXPECT_FALSE(polyline.isClosed());
}

TEST(NcPolylineTest, polygon)
{
    std::vector<nc::Vector2> points;
    points.push_back({0, 0});
    points.push_back({2, 0});
    points.push_back({0, 2});
    points.push_back({3, 3});
    points.push_back({2, 5});
    points.push_back({0, 0});

    NcPolyline ploygon(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon.pointCount(), 6);
    EXPECT_TRUE(ploygon.isClosed());
}