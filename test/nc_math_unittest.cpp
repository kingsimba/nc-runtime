#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_math.h"

using namespace nc;
using namespace std;

TEST(Math, hash)
{
    EXPECT_TRUE(Math_hashSizeT(1) != Math_hashSizeT(2));
#ifdef NC_ENVIRONMENT_64
    EXPECT_EQ(Math_hashU64(1), Math_hashSizeT(1));
#else
    EXPECT_EQ(Math_hashU32(1), Math_hashSizeT(1));
#endif
}

TEST(Math, wrapTwoPi)
{
    EXPECT_NEAR(Math_wrapTwoPi(NC_TWO_PI * 4 + 0.1f), 0.1f, 0.00001f);
    EXPECT_NEAR(Math_wrapTwoPi(-NC_TWO_PI * 4 + 0.1f), 0.1f, 0.00001f);
}

TEST(Math, turnAngle)
{
    EXPECT_NEAR(Math_turnAngle(-0.1f, 0.1f), 0.2f, 0.00001f);
    EXPECT_NEAR(Math_turnAngle(0.1f, -0.1f), -0.2f, 0.00001f);

    EXPECT_NEAR(Math_turnAngle(NC_TWO_PI - 0.1f, 0.1f), 0.2f, 0.00001f);
    EXPECT_NEAR(Math_turnAngle(0.1f, NC_TWO_PI - 0.1f), -0.2f, 0.00001f);
}

TEST(Math, Math_pointInPolygon)
{
    // 0 point
    {
        vector<Vector2> points;
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
    }

    // 1 point
    {
        vector<Vector2> points{{0, 0}};
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
    }

    // 2 points
    {
        vector<Vector2> points{{0, 0}, {10, 0}};
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(0, 0)));
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(5, 0)));
    }

    // more points
    {
        vector<Vector2> points{{0, 0}, {10, 0}, {10, 10}, {0, 10}};
        EXPECT_TRUE(Math_pointInPolygon(points.data(), points.size(), Vector2(5, 5)));
        EXPECT_TRUE(Math_pointInPolygon(points.data(), points.size(), Vector2(10, 5)));
        EXPECT_FALSE(Math_pointInPolygon(points.data(), points.size(), Vector2(10.01f, 5)));
    }
}

// #define POINT_COUNT 100000000
#define POINT_COUNT 100
static vector<Vector2> g_points;
static vector<Vector2> g_polygon{{90.3515625f, 67.47492238478702f},
                                 {81.2109375f, 65.2198939361321f},
                                 {79.453125f, 55.3791104480105f},
                                 {78.3984375f, 39.095962936305476f},
                                 {113.5546875f, 36.03133177633187f},
                                 {124.45312499999999f, 42.5530802889558f},
                                 {135.0f, 67.47492238478702f},
                                 {126.91406249999999f, 69.53451763078358f},
                                 {110.390625f, 72.91963546581484f},
                                 {100.8984375f, 73.42842364106816f},
                                 {95.2734375f, 73.42842364106816f},
                                 {79.453125f, 74.21198251594369f},
                                 {69.9609375f, 72.71190310803662f},
                                 {55.1953125f, 70.61261423801925f},
                                 {59.0625f, 68.00757101804004f},
                                 {70.3125f, 69.90011762668541f},
                                 {77.34374999999999f, 70.37785394109224f},
                                 {89.6484375f, 70.49557354093136f},
                                 {95.2734375f, 70.37785394109224f},
                                 {104.765625f, 68.39918004344189f},
                                 {110.0390625f, 65.5129625532949f},
                                 {104.0625f, 61.60639637138628f},
                                 {99.140625f, 55.57834467218206f},
                                 {94.21875f, 54.77534585936447f},
                                 {96.328125f, 61.938950426660604f},
                                 {96.328125f, 65.2198939361321f},
                                 {90.3515625f, 67.47492238478702f}};

class PointInPolygonTest : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        g_points.reserve(POINT_COUNT);
        for (int i = 0; i < POINT_COUNT; i++)
        {
            Vector2 v;
            v.x = (float)(rand() % 50) + 60;
            v.y = (float)(rand() % 30) + 30;
            g_points.push_back(v);
        }
    }
};

TEST_F(PointInPolygonTest, performance)
{
    for (Vector2 v : g_points)
    {
        Math_pointInPolygon(g_polygon.data(), g_polygon.size(), v);
    }
}

TEST(Math, linePointDistance)
{
    Vector2 p1 = {1.0f, 6.0f};
    Vector2 p2 = {5.0f, 3.0f};
    Vector2 proj;

    EXPECT_EQ(Math_linePointDistance({1.0f, 9.0f}, p1, p2, &proj), 2.4f);
    EXPECT_FLOAT_EQ(proj.x, -0.44f);
    EXPECT_FLOAT_EQ(proj.y, 7.08f);

    EXPECT_EQ(Math_linePointDistance({5.0f, 6.0f}, p1, p2, &proj), 2.4f);
    EXPECT_FLOAT_EQ(proj.x, 3.56f);
    EXPECT_FLOAT_EQ(proj.y, 4.08f);

    // parameter projectionOut is optional
    EXPECT_EQ(Math_linePointDistance({8.0f, 3.0f}, p1, p2, NULL), 1.8f);
}

TEST(Math, segmentPointDistance)
{
    Vector2 p1 = {1.0f, 6.0f};
    Vector2 p2 = {5.0f, 3.0f};
    Vector2 nearest;

    EXPECT_EQ(Math_segmentPointDistance({1.0f, 9.0f}, p1, p2, &nearest), 3.0f);
    EXPECT_FLOAT_EQ(nearest.x, 1.0f);
    EXPECT_FLOAT_EQ(nearest.y, 6.0f);

    EXPECT_EQ(Math_segmentPointDistance({5.0f, 6.0f}, p1, p2, &nearest), 2.4f);
    EXPECT_FLOAT_EQ(nearest.x, 3.56f);
    EXPECT_FLOAT_EQ(nearest.y, 4.08f);

    // parameter nearestPtOut is optional
    EXPECT_EQ(Math_segmentPointDistance({8.0f, 3.0f}, p1, p2, NULL), 3.0f);
}

TEST(Math, rectPointDistance)
{
    RectF rect(1.0f, 3.0f, 2.0f, 5.0f);
    Vector2 nearest;

    EXPECT_EQ(Math_rectPointDistance({1.2f, 3.4f}, rect, nullptr), 0.0f);
    EXPECT_EQ(Math_rectPointDistance({1.2f, 3.4f}, rect, &nearest), 0.0f);
    EXPECT_EQ(nearest, Vector2(1.2f, 3.4f));

    EXPECT_EQ(Math_rectPointDistance({1.2f, -3.4f}, rect, nullptr), 6.4f);
    EXPECT_EQ(Math_rectPointDistance({1.2f, -3.4f}, rect, &nearest), 6.4f);
    EXPECT_EQ(nearest, Vector2(1.2f, 3.0f));

    EXPECT_EQ(Math_rectPointDistance({6.0f, 0.0f}, rect, nullptr), 5.0f);
    EXPECT_EQ(Math_rectPointDistance({6.0f, 0.0f}, rect, &nearest), 5.0f);
    EXPECT_EQ(nearest, Vector2(2.0f, 3.0f));

    EXPECT_EQ(Math_rectPointDistance({6.0f, 4.2f}, rect, nullptr), 4.0f);
    EXPECT_EQ(Math_rectPointDistance({6.0f, 4.2f}, rect, &nearest), 4.0f);
    EXPECT_EQ(nearest, Vector2(2.0f, 4.2f));

    EXPECT_EQ(Math_rectPointDistance({5.0f, 9.0f}, rect, nullptr), 5.0f);
    EXPECT_EQ(Math_rectPointDistance({5.0f, 9.0f}, rect, &nearest), 5.0f);
    EXPECT_EQ(nearest, Vector2(2.0f, 5.0f));

    EXPECT_EQ(Math_rectPointDistance({1.9f, 6.0f}, rect, nullptr), 1.0f);
    EXPECT_EQ(Math_rectPointDistance({1.9f, 6.0f}, rect, &nearest), 1.0f);
    EXPECT_EQ(nearest, Vector2(1.9f, 5.0f));

    EXPECT_EQ(Math_rectPointDistance({-3.0f, 8.0f}, rect, nullptr), 5.0f);
    EXPECT_EQ(Math_rectPointDistance({-3.0f, 8.0f}, rect, &nearest), 5.0f);
    EXPECT_EQ(nearest, Vector2(1.0f, 5.0f));

    EXPECT_EQ(Math_rectPointDistance({-3.0f, 3.5f}, rect, nullptr), 4.0f);
    EXPECT_EQ(Math_rectPointDistance({-3.0f, 3.5f}, rect, &nearest), 4.0f);
    EXPECT_EQ(nearest, Vector2(1.0f, 3.5f));

    EXPECT_EQ(Math_rectPointDistance({-2.0f, -1.0f}, rect, nullptr), 5.0f);
    EXPECT_EQ(Math_rectPointDistance({-2.0f, -1.0f}, rect, &nearest), 5.0f);
    EXPECT_EQ(nearest, Vector2(1.0f, 3.0f));
}

TEST(Math, segmentsIntersect)
{
    Vector2 p1 = {4.0f, 12.0f};
    Vector2 p2 = {10.0f, 2.0f};
    Vector2 p3 = {10.0f, 10.0f};
    Vector2 p4 = {4.0f, 8.0f};

    EXPECT_TRUE(Math_segmentsIntersect(p1, p2, p3, p4));

    p4 = {8.0f, 8.0f};
    EXPECT_FALSE(Math_segmentsIntersect(p1, p2, p3, p4));

    p4 = {4.0f, 12.0f}; // the same as p1
    EXPECT_TRUE(Math_segmentsIntersect(p1, p2, p3, p4));

    p3 = {10.0f, 14.0f};
    EXPECT_TRUE(Math_segmentsIntersect(p1, p2, p3, p4));
}

TEST(Math, clipSegmentByRectFloatVersion)
{
    RectF area{10.0f, 10.0f, 20.0f, 20.0f};
    Vector2 p1, p2;

    p1 = {13.0f, 13.0f};
    p2 = {17.0f, 17.0f};

    EXPECT_TRUE(Math_clipSegmentByRect(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2(13.0f, 13.0f));
    EXPECT_EQ(p2, Vector2(17.0f, 17.0f));

    p1 = {5.0f, 18.0f};
    p2 = {15.0f, 30.0f};
    EXPECT_FALSE(Math_clipSegmentByRect(area, &p1, &p2));

    p1 = {11.0f, 19.0f};
    p2 = {19.0f, 21.0f};
    EXPECT_TRUE(Math_clipSegmentByRect(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2(11.0f, 19.0f));
    EXPECT_EQ(p2, Vector2(15.0f, 20.0f));

    p1 = {0.0f, 13.0f};
    p2 = {50.0f, 13.0f};
    EXPECT_TRUE(Math_clipSegmentByRect(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2(10.0f, 13.0f));
    EXPECT_EQ(p2, Vector2(20.0f, 13.0f));

    p1 = {8.0f, 19.0f};
    p2 = {16.0f, 7.0f};
    EXPECT_TRUE(Math_clipSegmentByRect(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2(10.0f, 16.0f));
    EXPECT_EQ(p2, Vector2(14.0f, 10.0f));

    p1 = {8.0f, 21.0f};
    p2 = {21.0f, 8.0f};
    EXPECT_TRUE(Math_clipSegmentByRect(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2(10.0f, 19.0f));
    EXPECT_EQ(p2, Vector2(19.0f, 10.0f));
}

TEST(Math, clipSegmentByRectIntVersion)
{
    Rect area{10, 10, 20, 20};
    Vector2i p1, p2;

    p1 = {13, 13};
    p2 = {17, 17};

    EXPECT_TRUE(Math_clipSegmentByRectI(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2i(13, 13));
    EXPECT_EQ(p2, Vector2i(17, 17));

    p1 = {5, 18};
    p2 = {15, 30};
    EXPECT_FALSE(Math_clipSegmentByRectI(area, &p1, &p2));

    p1 = {11, 19};
    p2 = {19, 21};
    EXPECT_TRUE(Math_clipSegmentByRectI(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2i(11, 19));
    EXPECT_EQ(p2, Vector2i(11, 19));

    p1 = {0, 13};
    p2 = {50, 13};
    EXPECT_TRUE(Math_clipSegmentByRectI(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2i(10, 13));
    EXPECT_EQ(p2, Vector2i(19, 13));

    p1 = {8, 19};
    p2 = {16, 7};
    EXPECT_TRUE(Math_clipSegmentByRectI(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2i(10, 16));
    EXPECT_EQ(p2, Vector2i(14, 10));

    p1 = {8, 21};
    p2 = {21, 8};
    EXPECT_TRUE(Math_clipSegmentByRectI(area, &p1, &p2));
    EXPECT_EQ(p1, Vector2i(10, 19));
    EXPECT_EQ(p2, Vector2i(19, 10));
}

TEST(Math, resampler)
{
    Resampler o(7, 30);

    int counter = 0;
    for (int i = 0; i < 300; i++)
    {
        if (o.sample())
        {
            counter++;
        }
    }

    EXPECT_EQ(counter, 70);
}

TEST(Math, resamplerDefault)
{
    Resampler o;

    int counter = 0;
    for (int i = 0; i < 100; i++)
    {
        if (o.sample())
        {
            counter++;
        }
    }

    EXPECT_EQ(counter, 100);
}
