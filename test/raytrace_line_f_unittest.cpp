#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/raytrace_line_f.h"
#include "nc_runtime/nc_image.h"

class CellGatherer
{
public:
    CellGatherer(vector<Vector2i>& cells) : m_cells(cells) {}
    inline void operator()(Vector2i cell) { m_cells.push_back(cell); }

private:
    vector<Vector2i>& m_cells;
};

TEST(RaytraceLineFTest, case1)
{
    vector<Vector2i> locs;
    CellGatherer cellGatherer(locs);

    raytraceLineF(cellGatherer, {9.926537f, 5.463142f}, {0.5734711f, 0.063133f});

    EXPECT_EQ(locs.size(), 15u);
    if (locs.size() == 15u)
    {
        EXPECT_EQ(locs[0], Vector2i(9, 5));
        EXPECT_EQ(locs[1], Vector2i(9, 4));
        EXPECT_EQ(locs[2], Vector2i(8, 4));
        EXPECT_EQ(locs[3], Vector2i(7, 4));
        EXPECT_EQ(locs[4], Vector2i(7, 3));
        EXPECT_EQ(locs[5], Vector2i(6, 3));
        EXPECT_EQ(locs[6], Vector2i(5, 3));
        EXPECT_EQ(locs[7], Vector2i(5, 2));
        EXPECT_EQ(locs[8], Vector2i(4, 2));
        EXPECT_EQ(locs[9], Vector2i(3, 2));
        EXPECT_EQ(locs[10], Vector2i(3, 1));
        EXPECT_EQ(locs[11], Vector2i(2, 1));
        EXPECT_EQ(locs[12], Vector2i(2, 0));
        EXPECT_EQ(locs[13], Vector2i(1, 0));
        EXPECT_EQ(locs[14], Vector2i(0, 0));
    }
}

TEST(RaytraceLineFTest, case2)
{
    vector<Vector2i> locs;
    CellGatherer cellGatherer(locs);

    raytraceLineF(cellGatherer, {1.234f, 4.345f}, {1.234f, 3.127f});

    EXPECT_EQ(locs.size(), 2u);
    if (locs.size() == 2u)
    {
        EXPECT_EQ(locs[0], Vector2i(1, 4));
        EXPECT_EQ(locs[1], Vector2i(1, 3));
    }

    locs.clear();
    raytraceLineF(cellGatherer, {3.1416f, 3.1416f}, {3.1416f, 3.1416f});
    EXPECT_EQ(locs.size(), 1u);
    if (locs.size() == 1u)
    {
        EXPECT_EQ(locs[0], Vector2i(3, 3));
    }

    locs.clear();
    raytraceLineF(cellGatherer, {3.9416f, 3.0416f}, {3.1416f, 3.8416f});
    EXPECT_EQ(locs.size(), 1u);
    if (locs.size() == 1u)
    {
        EXPECT_EQ(locs[0], Vector2i(3, 3));
    }

    locs.clear();
    raytraceLineF(cellGatherer, {4.1942f, 3.4167f}, {0.1416f, 3.8416f});
    EXPECT_EQ(locs.size(), 5u);
    if (locs.size() == 5u)
    {
        EXPECT_EQ(locs[0], Vector2i(4, 3));
        EXPECT_EQ(locs[1], Vector2i(3, 3));
        EXPECT_EQ(locs[2], Vector2i(2, 3));
        EXPECT_EQ(locs[3], Vector2i(1, 3));
        EXPECT_EQ(locs[4], Vector2i(0, 3));
    }
}

class ImageU8CellMarker
{
public:
    ImageU8CellMarker(NcImageU8* image, u8 pixel) : m_img(image), m_pixel(pixel) {}
    inline void operator()(nc::Vector2i cell) { m_img->mutablePixels()[cell.y * m_img->width() + cell.x] = m_pixel; }

private:
    NcImageU8* m_img;
    u8 m_pixel;
};

/*
    https://autoxing.atlassian.net/browse/RCSS-1077
    Costmap2D::raytraceLineF() 在某些特殊情况下引起内存非法访问
*/
TEST(RaytraceLineFTest, case3)
{
    auto img = NcImageU8::allocWithSize({200, 200});
    img->clear(0xFF);

    EXPECT_EQ(img->pixelAt(100, 100), 0xFF);
    EXPECT_EQ(img->pixelAt(150, 50), 0xFF);

    ImageU8CellMarker marker(img, 0);
    raytraceLineF(marker, {100.0f, 100.0f}, {150.0f, 50.0f});

    EXPECT_EQ(img->pixelAt(100, 100), 0);
    EXPECT_EQ(img->pixelAt(150, 50), 0);
}
