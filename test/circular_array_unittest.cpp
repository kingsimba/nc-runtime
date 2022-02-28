#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/circular_array.h"

TEST(FixedCircularArray, basic)
{
    FixedCircularArray<int, 3> arr;
    EXPECT_TRUE(arr.empty());
    EXPECT_FALSE(arr.full());

    arr.push(1);
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_FALSE(arr.empty());
    EXPECT_FALSE(arr.full());
    arr.push(2);
    EXPECT_EQ(arr.size(), 2u);
    EXPECT_FALSE(arr.empty());
    EXPECT_FALSE(arr.full());
    arr.push(3);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_FALSE(arr.empty());
    EXPECT_TRUE(arr.full());
    arr.push(4);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_FALSE(arr.empty());
    EXPECT_TRUE(arr.full());

    EXPECT_EQ(arr[0], 2);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 4);

    arr.push(5);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 4);
    EXPECT_EQ(arr[2], 5);
}
