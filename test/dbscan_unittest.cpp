#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/dbscan.h"

class Vector2Spec
{
public:
    static float distance(const nc::Vector2& l, const nc::Vector2& r)
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

    nc::Dbscan<nc::Vector2, Vector2Spec> dbscan(4, 0.2f);
    dbscan.run(points, &clusterIds);

    ASSERT_EQ(clusterIds.size(), 11);
    EXPECT_EQ(clusterIds[0], 1);
    EXPECT_EQ(clusterIds[1], 2);
    EXPECT_EQ(clusterIds[2], 0);
}