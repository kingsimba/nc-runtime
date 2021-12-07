#pragma once

#include "_raytrace.h"

template <typename ImageType>
class ImagePainter
{
public:
    ImagePainter(ImageType* image) : m_image(image) {}

    typedef typename ImageType::PixelType PixelType;

    class LineDrawer
    {
    public:
        LineDrawer(PixelType* pixels, PixelType color) : m_pixels(pixels), m_color(color) {}
        inline void operator()(int offset) { m_pixels[offset] = m_color; }

    private:
        PixelType* m_pixels;
        PixelType m_color;
    };

    void line(Vector2i from, Vector2i to, PixelType color)
    {
        LineDrawer drawer(m_image->mutablePixels(), color);
        raytraceLine(drawer, m_image->width(), from.x, from.y, to.x, to.y);
    }

private:
    ImageType* m_image;
};
