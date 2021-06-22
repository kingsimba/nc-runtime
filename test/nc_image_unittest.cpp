#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/nc_image.h"

class NcImageTest : public ::testing::Test
{
  public:
    static void SetUpTestCase()
    {
        // prepare RGBA image
        {
            m_image = NcImage::allocWithSize(Size{16, 16});
            Rgba8* pixels = m_image->mutablePixels();
            for (int i = 0; i < 256; i++)
            {
                pixels[i] = Rgba8{(u8)(255 - i), (u8)i, 0, 255};
            }
        }

        // prepare U8 image
        {
            m_image8 = NcImageU8::allocWithSize(Size{16, 16});
            u8* pixels = m_image8->mutablePixels();
            for (int i = 0; i < 256; i++)
            {
                pixels[i] = i % 256;
            }
        }
    }

  protected:
    static sp<NcImage> m_image;
    static sp<NcImageU8> m_image8;
};

sp<NcImage> NcImageTest::m_image;
sp<NcImageU8> NcImageTest::m_image8;

TEST_F(NcImageTest, basic)
{
    auto o = m_image;

    EXPECT_TRUE(o->saveAs("test_data/output/test.png"_str));

    auto loaded = NcImage::allocWithFileName("test_data/output/test.png");
    ASSERT_TRUE(loaded != NULL);
    EXPECT_EQ(loaded->size(), Size_make(16, 16));
    const Rgba8* pixels = loaded->pixels();
    EXPECT_EQ(pixels[0], Rgba8_make(255, 0, 0, 255));
    EXPECT_EQ(pixels[1], Rgba8_make(254, 1, 0, 255));
}

TEST_F(NcImageTest, u8)
{
    auto o = m_image8;

    EXPECT_TRUE(o->saveAs("test_data/output/test_greyscale.png"_str));

    auto loaded = NcImageU8::allocWithFileName("test_data/output/test_greyscale.png");
    ASSERT_TRUE(loaded != NULL);
    EXPECT_EQ(loaded->size(), Size_make(16, 16));
    const u8* pixels = loaded->pixels();
    EXPECT_EQ(pixels[0], 0);
    EXPECT_EQ(pixels[1], 1);
}

TEST_F(NcImageTest, u8Copy)
{
    auto o = NcImageU8::allocByCoping(m_image8.get());
    EXPECT_EQ(o->size(), Size_make(16, 16));
    const u8* pixels = o->pixels();
    EXPECT_EQ(pixels[0], 0);
    EXPECT_EQ(pixels[1], 1);
}
