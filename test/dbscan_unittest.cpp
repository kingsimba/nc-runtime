#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/dbscan.h"

class Vector2Spec
{
public:
    typedef nc::Vector2 PointType;

    static float distance(const PointType& l, const PointType& r)
    {
        return sqrt((r.x - l.x) * (r.x - l.x) + (r.y - l.y) * (r.y - l.y));
    }
};

TEST(DbscanTest, basic)
{
    std::vector<nc::Vector2> points = {{1.0f, 1.0f},  {-1.0f, 1.0f}, {0.0f, -2.0f}, {1.1f, 1.0f},
                                       {0.9f, 1.0f},  {1.0f, 1.1f},  {1.0f, 0.9f},  {-1.1f, 1.0f},
                                       {-0.9f, 1.0f}, {-1.0f, 1.1f}, {-1.0f, 0.9f}};

    std::vector<int> clusterIds;

    nc::Dbscan<Vector2Spec> dbscan(4, 0.2f);
    dbscan.run(points, &clusterIds);

    ASSERT_EQ(clusterIds.size(), 11u);

    EXPECT_EQ(clusterIds[0], 1);
    EXPECT_EQ(clusterIds[1], 2);
    EXPECT_EQ(clusterIds[2], nc::ClusterId::noise);
    EXPECT_EQ(clusterIds[3], 1);
    EXPECT_EQ(clusterIds[4], 1);
    EXPECT_EQ(clusterIds[5], 1);
    EXPECT_EQ(clusterIds[6], 1);
    EXPECT_EQ(clusterIds[7], 2);
    EXPECT_EQ(clusterIds[8], 2);
    EXPECT_EQ(clusterIds[9], 2);
    EXPECT_EQ(clusterIds[10], 2);
}