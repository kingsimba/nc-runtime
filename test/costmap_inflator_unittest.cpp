#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/costmap_inflator.h"
#include "nc_runtime/nc_image.h"

TEST(CostmapInflator, basic)
{
    auto o = NcImageU8::allocWithSize(Size{100, 100});
    auto pixels = o->mutablePixels();
    for (int i = 0; i < 10000; i++)
    {
        pixels[i] = (u8)(0);
    }
    pixels[100 * 50 + 50] = (u8)(125);
    pixels[100 * 50 + 60] = (u8)(125);
    pixels[100 * 50 + 70] = (u8)(125);
    pixels[100 * 60 + 50] = (u8)(125);

    pixels[100 * 30 + 30] = (u8)(254);
    pixels[100 * 40 + 40] = (u8)(254);

    pixels[100 * 10 + 40] = (u8)(254);

    CostmapInflatorParams params;
    params.costScalingFactor = 20;
    params.resolution = 0.05;
    params.inflationRadius = 0.6;
    params.inscribedRadius = 0.2;
    CostmapInflator inflator(params);
    Rect region = Rect_make(20, 20, 80, 80);
    auto d = inflator.inflate(o.get(), region);

    EXPECT_TRUE(d->saveAs("test_data/output/test_inflate.png"_str));
}

TEST(CostmapInflator, officeArea)
{
    // real image of office area
    auto image = NcImageU8::allocWithFileName("test_data/costmap_original.png");
    ASSERT_TRUE(image != NULL);

    CostmapInflatorParams params;
    params.costScalingFactor = 2;
    params.resolution = 0.05f;
    params.inflationRadius = 1.0f;
    params.inscribedRadius = 0.462f / 2.0f; // the inscribed radius of the robot
    CostmapInflator inflator(params);
    Rect region = image->area();
    auto d = inflator.inflate(image.get(), region);
    EXPECT_TRUE(d->saveAs("test_data/output/test_inflate_office.png"_str));
}