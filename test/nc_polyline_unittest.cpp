#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_polyline.h"

using namespace std;
using namespace nc;

TEST(NcPolylineTest, polyline)
{
    vector<Vector2> points{{0, 0}, {1, 2}, {3, 4}};

    // create on heap
    sp<NcPolyline> polyline = NcPolyline::alloc(points.data(), (int)points.size(), false);
    EXPECT_EQ(polyline->points()[0], Vector2(0, 0));
    EXPECT_EQ(polyline->points()[1], Vector2(1, 2));
    EXPECT_EQ(polyline->points()[2], Vector2(3, 4));
    EXPECT_EQ(polyline->pointCount(), 3);
    EXPECT_FALSE(polyline->isClosed());
}

TEST(NcPolylineTest, polygon)
{
    vector<Vector2> points{{0, 0}, {1, 0}, {0, 2}};

    // create on stack
    NcPolyline ploygon = NcPolyline::makeWithPointsNoCopy(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon.pointCount(), 3);
    EXPECT_TRUE(ploygon.isClosed());
}

TEST(NcPolylineTest, isPointInPolygon)
{
    vector<Vector2> points{{0, 0}, {1, 0}, {0, 2}};

    sp<NcPolyline> ploygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    EXPECT_EQ(ploygon->pointCount(), 3);
    EXPECT_TRUE(ploygon->isClosed());
    EXPECT_TRUE(ploygon->testPoint({0.5f, 0.6f}));
    EXPECT_TRUE(ploygon->testPoint({0.001f, 1.99f}));
    EXPECT_FALSE(ploygon->testPoint({1.f, 2.f}));
    EXPECT_FALSE(ploygon->testPoint({0.f, 2.1f}));
}

TEST(NcPolylineTest, calculateBBox)
{
    vector<Vector2> points{{0, 0}, {3, 0}, {3, 3}, {0, 3}};

    sp<NcPolyline> polygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    RectF boundingBox = polygon->calculateBBox();
    EXPECT_EQ(boundingBox.left, .0f);
    EXPECT_EQ(boundingBox.right, 3.0f);
    EXPECT_EQ(boundingBox.top, .0f);
    EXPECT_EQ(boundingBox.bottom, 3.0f);
}

TEST(NcPolylineTest, calculateCentroid)
{
    vector<Vector2> points1{{0, 0}, {3, 0}, {3, 3}, {0, 3}};
    sp<NcPolyline> polygon1 = NcPolyline::alloc(points1.data(), (int)points1.size(), true);
    EXPECT_EQ(polygon1->calculateCentroid(), Vector2(1.5f, 1.5f));

    // case with duplicate points
    vector<Vector2> points2{{0, 0}, {3, 0}, {3, 3}, {3, 3}, {3, 3}, {3, 3}, {0, 3}};
    sp<NcPolyline> polygon2 = NcPolyline::alloc(points2.data(), (int)points2.size(), true);
    EXPECT_EQ(polygon2->calculateCentroid(), Vector2(1.5f, 1.5f));
}

TEST(NcPolylineTest, calculateCentroidBugCase1)
{
    vector<Vector2> points{{1, 2}, {4, 2}, {4, 5}, {1, 5}};
    sp<NcPolyline> polygon = NcPolyline::alloc(points.data(), (int)points.size(), true);
    EXPECT_EQ(polygon->calculateCentroid(), Vector2(2.5f, 3.5f));
}