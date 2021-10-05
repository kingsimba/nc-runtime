#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_polyline.h"

TEST(NcPolylineTest, polyline)
{
    std::vector<nc::Vector2> points{{0, 0}, {1, 2}, {3, 4}};

    sp<NcPolyline> polyline = NcPolyline::alloc(points.data(), (int)points.size(), false);
    EXPECT_EQ(polyline->points()[0], Vector2(0, 0));
    EXPECT_EQ(polyline->points()[1], Vector2(1, 2));
    EXPECT_EQ(polyline->points()[2], Vector2(3, 4));
    EXPECT_EQ(polyline->pointCount(), 3);
    EXPECT_FALSE(polyline->isClosed());
}

TEST(NcPolylineTest, polygon)
{
    std::vector<nc::Vector2> points{{0, 0}, {1, 0}, {0, 2}};

    sp<NcPolyline> ploygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon->pointCount(), 3);
    EXPECT_TRUE(ploygon->isClosed());
}

TEST(NcPolylineTest, isPointInConvexPolygon)
{
    std::vector<nc::Vector2> points{{0, 0}, {1, 0}, {0, 2}};

    sp<NcPolyline> ploygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon->pointCount(), 3);
    EXPECT_TRUE(ploygon->isClosed());
    EXPECT_TRUE(ploygon->testPoint({0.5f, 0.6f}));
    EXPECT_TRUE(ploygon->testPoint({0.001f, 1.99f}));
    EXPECT_FALSE(ploygon->testPoint({1.f, 2.f}));
    EXPECT_FALSE(ploygon->testPoint({0.f, 2.1f}));
}
