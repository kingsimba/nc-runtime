#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/image_painter.h"
#include "nc_runtime/nc_image.h"

TEST(ImagePainterTest, basic)
{
    auto img = NcImageU8::allocWithSize({100, 100});
    img->clear(0);

    ImagePainter<NcImageU8> p(img.get());
    p.line({10, 10}, {20, 40}, 0xFF);
    p.line({20, 40}, {99, 99}, 0x80);

    EXPECT_EQ(img->pixelAt(0, 0), 0);

    EXPECT_EQ(img->pixelAt(10, 10), 0xFF);
    EXPECT_EQ(img->pixelAt(10, 11), 0xFF);
    EXPECT_EQ(img->pixelAt(11, 12), 0xFF);
    EXPECT_EQ(img->pixelAt(11, 13), 0xFF);

    EXPECT_EQ(img->pixelAt(20, 40), 0x80);
    EXPECT_EQ(img->pixelAt(99, 99), 0x80);

    img->saveAs("test_data/output/line_painter.png");
}
